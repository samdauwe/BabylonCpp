#include <babylon/materials/node/blocks/pbr/pbr_metallic_roughness_block.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material_flags.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/node/blocks/fragment/perturb_normal_block.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/blocks/pbr/ambient_occlusion_block.h>
#include <babylon/materials/node/blocks/pbr/anisotropy_block.h>
#include <babylon/materials/node/blocks/pbr/clear_coat_block.h>
#include <babylon/materials/node/blocks/pbr/reflection_block.h>
#include <babylon/materials/node/blocks/pbr/reflectivity_block.h>
#include <babylon/materials/node/blocks/pbr/refraction_block.h>
#include <babylon/materials/node/blocks/pbr/sheen_block.h>
#include <babylon/materials/node/blocks/pbr/sub_surface_block.h>
#include <babylon/materials/node/node_material.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/pbr/pbr_base_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/brdf_texture_tools.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

// clang-format off
static const std::unordered_map<std::string, std::vector<std::string>> mapOutputToVariable{
  {"ambient",      {"finalAmbient", ""}},
  {"diffuse",      {"finalDiffuse", ""}},
  {"specular",     {"finalSpecularScaled",                         "!defined(UNLIT) && defined(SPECULARTERM)"}},
  {"sheenDir",     {"finalSheenScaled",                            "!defined(UNLIT) && defined(SHEEN)"}},
  {"clearcoatDir", {"finalClearCoatScaled",                        "!defined(UNLIT) && defined(CLEARCOAT)"}},
  {"diffuseInd",   {"finalIrradiance",                             "!defined(UNLIT) && defined(REFLECTION)"}},
  {"specularInd",  {"finalRadianceScaled",                         "!defined(UNLIT) && defined(REFLECTION)"}},
  {"sheenInd",     {"sheenOut.finalSheenRadianceScaled",           "!defined(UNLIT) && defined(REFLECTION) && defined(SHEEN) && defined(ENVIRONMENTBRDF)"}},
  {"clearcoatInd", {"clearcoatOut.finalClearCoatRadianceScaled",   "!defined(UNLIT) && defined(REFLECTION) && defined(CLEARCOAT)"}},
  { "refraction",  {"subSurfaceOut.finalRefraction",               "!defined(UNLIT) && defined(SS_REFRACTION)"}},
  { "lighting",    {"finalColor.rgb", ""}},
  { "shadow",      {"shadow", ""}},
  { "alpha",       {"alpha", ""}}
};
// clang-format on

static bool mapOutputToVariableContainsName(const std::string& name)
{
  for (const auto& item : mapOutputToVariable) {
    if (item.first == name) {
      return true;
    }
  }
  return false;
}

PBRMetallicRoughnessBlock::PBRMetallicRoughnessBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::VertexAndFragment}
    , light{nullptr}
    , directIntensity{1.f}
    , environmentIntensity{1.f}
    , specularIntensity{1.f}
    , lightFalloff{0}
    , useAlphaFromAlbedoTexture{false}
    , useAlphaTest{false}
    , alphaTestCutoff{0.5f}
    , useAlphaBlending{false}
    , opacityRGB{false}
    , useRadianceOverAlpha{true}
    , useSpecularOverAlpha{true}
    , enableSpecularAntiAliasing{false}
    , realTimeFiltering{false}
    , realTimeFilteringQuality{Constants::TEXTURE_FILTERING_QUALITY_LOW}
    , useEnergyConservation{true}
    , useRadianceOcclusion{true}
    , useHorizonOcclusion{true}
    , unlit{false}
    , forceNormalForward{false}
    , debugMode{0}
    , debugLimit{0.f}
    , debugFactor{1.f}
    , worldPosition{this, &PBRMetallicRoughnessBlock::get_worldPosition}
    , worldNormal{this, &PBRMetallicRoughnessBlock::get_worldNormal}
    , perturbedNormal{this, &PBRMetallicRoughnessBlock::get_perturbedNormal}
    , cameraPosition{this, &PBRMetallicRoughnessBlock::get_cameraPosition}
    , baseColor{this, &PBRMetallicRoughnessBlock::get_baseColor}
    , opacityTexture{this, &PBRMetallicRoughnessBlock::get_opacityTexture}
    , ambientColor{this, &PBRMetallicRoughnessBlock::get_ambientColor}
    , reflectivity{this, &PBRMetallicRoughnessBlock::get_reflectivity}
    , ambientOcc{this, &PBRMetallicRoughnessBlock::get_ambientOcc}
    , reflection{this, &PBRMetallicRoughnessBlock::get_reflection}
    , sheen{this, &PBRMetallicRoughnessBlock::get_sheen}
    , clearcoat{this, &PBRMetallicRoughnessBlock::get_clearcoat}
    , subsurface{this, &PBRMetallicRoughnessBlock::get_subsurface}
    , anisotropy{this, &PBRMetallicRoughnessBlock::get_anisotropy}
    , view{this, &PBRMetallicRoughnessBlock::get_view}
    , ambient{this, &PBRMetallicRoughnessBlock::get_ambient}
    , diffuse{this, &PBRMetallicRoughnessBlock::get_diffuse}
    , specular{this, &PBRMetallicRoughnessBlock::get_specular}
    , sheenDir{this, &PBRMetallicRoughnessBlock::get_sheenDir}
    , clearcoatDir{this, &PBRMetallicRoughnessBlock::get_clearcoatDir}
    , diffuseIndirect{this, &PBRMetallicRoughnessBlock::get_diffuseIndirect}
    , specularIndirect{this, &PBRMetallicRoughnessBlock::get_specularIndirect}
    , sheenIndirect{this, &PBRMetallicRoughnessBlock::get_sheenIndirect}
    , clearcoatIndirect{this, &PBRMetallicRoughnessBlock::get_clearcoatIndirect}
    , refraction{this, &PBRMetallicRoughnessBlock::get_refraction}
    , lighting{this, &PBRMetallicRoughnessBlock::get_lighting}
    , shadow{this, &PBRMetallicRoughnessBlock::get_shadow}
    , alpha{this, &PBRMetallicRoughnessBlock::get_alpha}
    , _lightId{0}
    , _scene{nullptr}
    , _environmentBRDFTexture{nullptr}
{
  _isUnique = true;
}

PBRMetallicRoughnessBlock::~PBRMetallicRoughnessBlock() = default;

void PBRMetallicRoughnessBlock::RegisterConnections(
  const PBRMetallicRoughnessBlockPtr& pbrMetallicRoughnessBlock)
{
  pbrMetallicRoughnessBlock->registerInput("worldPosition",
                                           NodeMaterialBlockConnectionPointTypes::Vector4, false,
                                           NodeMaterialBlockTargets::Vertex);
  pbrMetallicRoughnessBlock->registerInput("worldNormal",
                                           NodeMaterialBlockConnectionPointTypes::Vector4, false,
                                           NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerInput("perturbedNormal",
                                           NodeMaterialBlockConnectionPointTypes::Vector4, true,
                                           NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerInput("cameraPosition",
                                           NodeMaterialBlockConnectionPointTypes::Vector3, false,
                                           NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerInput("baseColor",
                                           NodeMaterialBlockConnectionPointTypes::Color4, true,
                                           NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerInput("opacityTexture",
                                           NodeMaterialBlockConnectionPointTypes::Color4, true,
                                           NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerInput("ambientColor",
                                           NodeMaterialBlockConnectionPointTypes::Color3, true,
                                           NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerInput(
    "reflectivity", NodeMaterialBlockConnectionPointTypes::Object, false,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "reflectivity", pbrMetallicRoughnessBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> ReflectivityBlockPtr {
        return ReflectivityBlock::New(iName);
      },
      "ReflectivityBlock"));
  pbrMetallicRoughnessBlock->registerInput(
    "ambientOcc", NodeMaterialBlockConnectionPointTypes::Object, true,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "ambientOcc", pbrMetallicRoughnessBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> AmbientOcclusionBlockPtr {
        return AmbientOcclusionBlock::New(iName);
      },
      "AOBlock"));
  pbrMetallicRoughnessBlock->registerInput(
    "reflection", NodeMaterialBlockConnectionPointTypes::Object, true,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "reflection", pbrMetallicRoughnessBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> ReflectionBlockPtr { return ReflectionBlock::New(iName); },
      "ReflectionBlock"));
  pbrMetallicRoughnessBlock->registerInput(
    "sheen", NodeMaterialBlockConnectionPointTypes::Object, true,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "sheen", pbrMetallicRoughnessBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> SheenBlockPtr { return SheenBlock::New(iName); },
      "SheenBlock"));
  pbrMetallicRoughnessBlock->registerInput(
    "clearcoat", NodeMaterialBlockConnectionPointTypes::Object, true,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "clearcoat", pbrMetallicRoughnessBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> ClearCoatBlockPtr { return ClearCoatBlock::New(iName); },
      "ClearCoatBlock"));
  pbrMetallicRoughnessBlock->registerInput(
    "subsurface", NodeMaterialBlockConnectionPointTypes::Object, true,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "subsurface", pbrMetallicRoughnessBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> SubSurfaceBlockPtr { return SubSurfaceBlock::New(iName); },
      "SubSurfaceBlock"));
  pbrMetallicRoughnessBlock->registerInput(
    "anisotropy", NodeMaterialBlockConnectionPointTypes::Object, true,
    NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "anisotropy", pbrMetallicRoughnessBlock, NodeMaterialConnectionPointDirection::Input,
      [](const std::string& iName) -> AnisotropyBlockPtr { return AnisotropyBlock::New(iName); },
      "AnisotropyBlock"));
  pbrMetallicRoughnessBlock->registerInput("view", NodeMaterialBlockConnectionPointTypes::Matrix,
                                           true);

  pbrMetallicRoughnessBlock->registerOutput(
    "ambient", NodeMaterialBlockConnectionPointTypes::Color3, NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput(
    "diffuse", NodeMaterialBlockConnectionPointTypes::Color3, NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput(
    "specular", NodeMaterialBlockConnectionPointTypes::Color3, NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput(
    "sheenDir", NodeMaterialBlockConnectionPointTypes::Color3, NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput("clearcoatDir",
                                            NodeMaterialBlockConnectionPointTypes::Color3,
                                            NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput("diffuseInd",
                                            NodeMaterialBlockConnectionPointTypes::Color3,
                                            NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput("specularInd",
                                            NodeMaterialBlockConnectionPointTypes::Color3,
                                            NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput(
    "sheenInd", NodeMaterialBlockConnectionPointTypes::Color3, NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput("clearcoatInd",
                                            NodeMaterialBlockConnectionPointTypes::Color3,
                                            NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput("refraction",
                                            NodeMaterialBlockConnectionPointTypes::Color3,
                                            NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput(
    "lighting", NodeMaterialBlockConnectionPointTypes::Color3, NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput("shadow", NodeMaterialBlockConnectionPointTypes::Float,
                                            NodeMaterialBlockTargets::Fragment);
  pbrMetallicRoughnessBlock->registerOutput("alpha", NodeMaterialBlockConnectionPointTypes::Float,
                                            NodeMaterialBlockTargets::Fragment);
}

void PBRMetallicRoughnessBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("vLightingIntensity");

  state._excludeVariableName("geometricNormalW");
  state._excludeVariableName("normalW");
  state._excludeVariableName("faceNormal");

  state._excludeVariableName("albedoOpacityOut");
  state._excludeVariableName("surfaceAlbedo");
  state._excludeVariableName("alpha");

  state._excludeVariableName("NdotVUnclamped");
  state._excludeVariableName("NdotV");
  state._excludeVariableName("alphaG");
  state._excludeVariableName("AARoughnessFactors");
  state._excludeVariableName("environmentBrdf");
  state._excludeVariableName("ambientMonochrome");
  state._excludeVariableName("seo");
  state._excludeVariableName("eho");

  state._excludeVariableName("environmentRadiance");
  state._excludeVariableName("irradianceVector");
  state._excludeVariableName("environmentIrradiance");

  state._excludeVariableName("diffuseBase");
  state._excludeVariableName("specularBase");
  state._excludeVariableName("preInfo");
  state._excludeVariableName("info");
  state._excludeVariableName("shadow");

  state._excludeVariableName("finalDiffuse");
  state._excludeVariableName("finalAmbient");
  state._excludeVariableName("ambientOcclusionForDirectDiffuse");

  state._excludeVariableName("finalColor");

  state._excludeVariableName("vClipSpacePosition");
  state._excludeVariableName("vDebugMode");
}

std::string PBRMetallicRoughnessBlock::getClassName() const
{
  return "PBRMetallicRoughnessBlock";
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_worldPosition()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_worldNormal()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_perturbedNormal()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_cameraPosition()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_baseColor()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_opacityTexture()
{
  return _inputs[5];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_ambientColor()
{
  return _inputs[6];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_reflectivity()
{
  return _inputs[7];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_ambientOcc()
{
  return _inputs[8];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_reflection()
{
  return _inputs[9];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_sheen()
{
  return _inputs[10];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_clearcoat()
{
  return _inputs[11];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_subsurface()
{
  return _inputs[12];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_anisotropy()
{
  return _inputs[13];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_view()
{
  return _inputs[14];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_ambient()
{
  return _outputs[0];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_diffuse()
{
  return _outputs[1];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_specular()
{
  return _outputs[2];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_sheenDir()
{
  return _outputs[3];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_clearcoatDir()
{
  return _outputs[4];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_diffuseIndirect()
{
  return _outputs[5];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_specularIndirect()
{
  return _outputs[6];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_sheenIndirect()
{
  return _outputs[7];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_clearcoatIndirect()
{
  return _outputs[8];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_refraction()
{
  return _outputs[9];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_lighting()
{
  return _outputs[10];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_shadow()
{
  return _outputs[11];
}

NodeMaterialConnectionPointPtr& PBRMetallicRoughnessBlock::get_alpha()
{
  return _outputs[12];
}

void PBRMetallicRoughnessBlock::autoConfigure(const NodeMaterialPtr& material)
{
  if (!cameraPosition()->isConnected()) {
    auto cameraPositionInput
      = material->getInputBlockByPredicate([](const InputBlockPtr& b) -> bool {
          return b->systemValue() == NodeMaterialSystemValues::CameraPosition;
        });

    if (!cameraPositionInput) {
      cameraPositionInput = InputBlock::New("cameraPosition");
      cameraPositionInput->setAsSystemValue(NodeMaterialSystemValues::CameraPosition);
    }
    cameraPositionInput->output()->connectTo(cameraPosition);
  }
}

void PBRMetallicRoughnessBlock::prepareDefines(AbstractMesh* mesh,
                                               const NodeMaterialPtr& nodeMaterial,
                                               NodeMaterialDefines& defines, bool /*useInstances*/,
                                               SubMesh* /*subMesh*/)
{
  // General
  defines.setValue("PBR", true);
  defines.setValue("METALLICWORKFLOW", true);
  defines.setValue("DEBUGMODE", debugMode, true);
  defines.setValue("NORMALXYSCALE", true);
  defines.setValue("BUMP", perturbedNormal()->isConnected(), true);
  defines.setValue("LODBASEDMICROSFURACE", _scene->getEngine()->getCaps().textureLOD);

  // Albedo & Opacity
  defines.setValue("OPACITY", opacityTexture()->isConnected(), true);

  // Lighting & colors
  if (lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_STANDARD) {
    defines.setValue("USEPHYSICALLIGHTFALLOFF", false);
    defines.setValue("USEGLTFLIGHTFALLOFF", false);
  }
  else if (lightFalloff == PBRBaseMaterial::LIGHTFALLOFF_GLTF) {
    defines.setValue("USEPHYSICALLIGHTFALLOFF", false);
    defines.setValue("USEGLTFLIGHTFALLOFF", true);
  }
  else {
    defines.setValue("USEPHYSICALLIGHTFALLOFF", true);
    defines.setValue("USEGLTFLIGHTFALLOFF", false);
  }

  // Transparency
  defines.setValue("ALPHABLEND", useAlphaBlending, true);
  defines.setValue("ALPHAFROMALBEDO", useAlphaFromAlbedoTexture, true);
  defines.setValue("ALPHATEST", useAlphaTest, true);
  defines.setValue("ALPHATESTVALUE", alphaTestCutoff, true);
  defines.setValue("OPACITYRGB", opacityRGB, true);

  // Rendering
  defines.setValue("RADIANCEOVERALPHA", useRadianceOverAlpha, true);
  defines.setValue("SPECULAROVERALPHA", useSpecularOverAlpha, true);
  defines.setValue("SPECULARAA",
                   _scene->getEngine()->getCaps().standardDerivatives && enableSpecularAntiAliasing,
                   true);
  defines.setValue("REALTIME_FILTERING", realTimeFiltering, true);
  defines.setValue("NUM_SAMPLES", realTimeFilteringQuality, true);

  if (_scene->getEngine()->webGLVersion > 1) {
    defines.setValue("NUM_SAMPLES", realTimeFilteringQuality, true);
  }

  // Advanced
  defines.setValue("BRDF_V_HEIGHT_CORRELATED", true);
  defines.setValue("MS_BRDF_ENERGY_CONSERVATION", useEnergyConservation, true);
  defines.setValue("RADIANCEOCCLUSION", useRadianceOcclusion, true);
  defines.setValue("HORIZONOCCLUSION", useHorizonOcclusion, true);
  defines.setValue("UNLIT", unlit, true);
  defines.setValue("FORCENORMALFORWARD", forceNormalForward, true);

  if (_environmentBRDFTexture && MaterialFlags::ReflectionTextureEnabled()) {
    defines.setValue("ENVIRONMENTBRDF", true);
    defines.setValue("ENVIRONMENTBRDF_RGBD", _environmentBRDFTexture->isRGBD(), true);
  }
  else {
    defines.setValue("ENVIRONMENTBRDF", false);
    defines.setValue("ENVIRONMENTBRDF_RGBD", false);
  }

  if (!defines._areLightsDirty) {
    return;
  }

  const auto scene = mesh->getScene();

  if (!light) {
    // Lights
    MaterialHelper::PrepareDefinesForLights(scene, mesh, defines, true,
                                            nodeMaterial->maxSimultaneousLights);
    defines._needNormals = true;

    // Multiview
    MaterialHelper::PrepareDefinesForMultiview(scene, defines);
  }
  else {
    PrepareDefinesForLightsState state = {
      false, // needNormals
      false, // needRebuild
      false, // shadowEnabled
      false, // specularEnabled
      false  // lightmapMode
    };

    MaterialHelper::PrepareDefinesForLight(scene, mesh, light, _lightId, defines, true, state);

    if (state.needRebuild) {
      defines.rebuild();
    }
  }
}

void PBRMetallicRoughnessBlock::updateUniformsAndSamples(NodeMaterialBuildState& state,
                                                         const NodeMaterialPtr& nodeMaterial,
                                                         const NodeMaterialDefines& defines,
                                                         std::vector<std::string>& uniformBuffers)
{
  IEffectCreationOptions options;
  options.uniformsNames         = state.uniforms;
  options.uniformBuffersNames   = uniformBuffers;
  options.samplers              = state.samplers;
  options.materialDefines       = const_cast<NodeMaterialDefines*>(&defines);
  options.maxSimultaneousLights = nodeMaterial->maxSimultaneousLights;

  MaterialHelper::PrepareUniformsAndSamplersList(options);
}

void PBRMetallicRoughnessBlock::bind(Effect* effect, const NodeMaterialPtr& nodeMaterial,
                                     Mesh* mesh, SubMesh* /*subMesh*/)
{
  if (!mesh) {
    return;
  }

  const auto scene = mesh->getScene();

  if (!light) {
    MaterialHelper::BindLights(scene, mesh, effect, true, nodeMaterial->maxSimultaneousLights);
  }
  else {
    MaterialHelper::BindLight(light, _lightId, scene, effect, true);
  }

  effect->setTexture(_environmentBrdfSamplerName, _environmentBRDFTexture);

  effect->setFloat2("vDebugMode", debugLimit, debugFactor);

  const auto ambientScene = _scene->ambientColor;

  /* if (ambientScene) */ {
    effect->setColor3("ambientFromScene", ambientScene);
  }

  const auto invertNormal
    = (scene->useRightHandedSystem() == (scene->_mirroredCameraPosition != nullptr));

  effect->setFloat(_invertNormalName, invertNormal ? -1.f : 1.f);

  effect->setFloat4("vLightingIntensity", directIntensity, 1.f,
                    environmentIntensity * _scene->environmentIntensity(), specularIntensity);
}

void PBRMetallicRoughnessBlock::_injectVertexCode(NodeMaterialBuildState& state)
{
  const auto& worldPos = worldPosition();
  const auto iComments = StringTools::printf("//%s", name().c_str());

  // Declaration
  if (!light) { // Emit for all lights
    EmitFunctionFromIncludeOptions options;
    options.repeatKey = "maxSimultaneousLights";
    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightUboDeclaration" :
                                                                 "lightFragmentDeclaration",
                                   iComments, options);
    _lightId = 0;

    state.sharedData->dynamicUniformBlocks.emplace_back(shared_from_this());
  }
  else {
    _lightId
      = (stl_util::contains(state.counters, "lightCounter") ? state.counters["lightCounter"] : -1)
        + 1;
    state.counters["lightCounter"] = _lightId;

    EmitFunctionFromIncludeOptions options;
    options.replaceStrings = {{R"({X})", std::to_string(_lightId)}};
    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightUboDeclaration" :
                                                                 "lightFragmentDeclaration",
                                   iComments, options, std::to_string(_lightId));
  }

  // Inject code in vertex
  const auto worldPosVaryingName
    = StringTools::printf("v_%s", worldPos->associatedVariableName().c_str());
  if (state._emitVaryingFromString(worldPosVaryingName, "vec4")) {
    state.compilationString += StringTools::printf("%s = %s;\r\n", worldPosVaryingName.c_str(),
                                                   worldPos->associatedVariableName().c_str());
  }

  const auto reflectionBlock
    = (reflection()->isConnected() && reflection()->connectedPoint()) ?
        std::static_pointer_cast<ReflectionBlock>(reflection()->connectedPoint()->ownerBlock()) :
        nullptr;

  state.compilationString += reflectionBlock ? reflectionBlock->handleVertexSide(state) : "";

  state._emitUniformFromString("vDebugMode", "vec2", "defined(IGNORE) || DEBUGMODE > 0");
  state._emitUniformFromString("ambientFromScene", "vec3");

  if (state._emitVaryingFromString("vClipSpacePosition", "vec4",
                                   "defined(IGNORE) || DEBUGMODE > 0")) {
    state._injectAtEnd += "#if DEBUGMODE > 0\r\n";
    state._injectAtEnd += "vClipSpacePosition = gl_Position;\r\n";
    state._injectAtEnd += "#endif\r\n";
  }

  if (light) {
    EmitCodeFromIncludeOptions options;
    options.replaceStrings
      = {{R"({X})", std::to_string(_lightId)}, {R"(worldPos)", worldPos->associatedVariableName()}};
    state.compilationString += state._emitCodeFromInclude("shadowsVertex", iComments, options);
  }
  else {
    state.compilationString
      += StringTools::printf("vec4 worldPos = %s;\r\n", worldPos->associatedVariableName().c_str());
    if (view()->isConnected()) {
      state.compilationString
        += StringTools::printf("mat4 view = %s;\r\n", view()->associatedVariableName().c_str());
    }
    EmitCodeFromIncludeOptions options;
    options.repeatKey = "maxSimultaneousLights";
    state.compilationString += state._emitCodeFromInclude("shadowsVertex", iComments, options);
  }
}

std::string PBRMetallicRoughnessBlock::getAlbedoOpacityCode() const
{
  std::string code = "albedoOpacityOutParams albedoOpacityOut;\r\n";

  const auto albedoColor
    = baseColor()->isConnected() ? baseColor()->associatedVariableName() : "vec4(1., 1., 1., 1.)";
  const auto iOpacityTexture
    = opacityTexture()->isConnected() ? opacityTexture()->associatedVariableName() : "";

  code += StringTools::printf(
    R"(albedoOpacityBlock(
            %s,
        #ifdef ALBEDO
            vec4(1.),
            vec2(1., 1.),
        #endif
        #ifdef OPACITY
            %s,
            vec2(1., 1.),
        #endif
            albedoOpacityOut
        );

        vec3 surfaceAlbedo = albedoOpacityOut.surfaceAlbedo;
        float alpha = albedoOpacityOut.alpha;\r\n`
      )",
    albedoColor.c_str(), iOpacityTexture.c_str());

  return code;
}

PBRMetallicRoughnessBlock& PBRMetallicRoughnessBlock::_buildBlock(NodeMaterialBuildState& state)
{
  NodeMaterialBlock::_buildBlock(state);

  _scene = state.sharedData->scene;

  if (!_environmentBRDFTexture) {
    _environmentBRDFTexture = BRDFTextureTools::GetEnvironmentBRDFTexture(_scene);
  }

  const auto reflectionBlock
    = reflection()->isConnected() && reflection()->connectedPoint() ?
        std::static_pointer_cast<ReflectionBlock>(reflection()->connectedPoint()->ownerBlock()) :
        nullptr;

  if (reflectionBlock) {
    // Need those variables to be setup when calling _injectVertexCode
    reflectionBlock->worldPositionConnectionPoint  = worldPosition;
    reflectionBlock->cameraPositionConnectionPoint = cameraPosition;
    reflectionBlock->worldNormalConnectionPoint    = worldNormal;
  }

  if (state.target != NodeMaterialBlockTargets::Fragment) {
    // Vertex
    _injectVertexCode(state);

    return *this;
  }

  // Fragment
  state.sharedData->bindableBlocks.emplace_back(shared_from_this());
  state.sharedData->blocksWithDefines.emplace_back(shared_from_this());

  const auto iComments       = StringTools::printf("//%s", name().c_str());
  const auto worldPosVarName = "v_" + worldPosition()->associatedVariableName();
  const auto normalShading   = perturbedNormal();

  _environmentBrdfSamplerName = state._getFreeVariableName("environmentBrdfSampler");

  state._emit2DSampler(_environmentBrdfSamplerName);

  state.sharedData->hints.needAlphaBlending
    = state.sharedData->hints.needAlphaBlending || useAlphaBlending;
  state.sharedData->hints.needAlphaTesting
    = state.sharedData->hints.needAlphaTesting || useAlphaTest;

  state._emitExtension("lod", "#extension GL_EXT_shader_texture_lod : enable",
                       "defined(LODBASEDMICROSFURACE)");

  //
  // Includes
  //
  if (!light) { // Emit for all lights
    EmitFunctionFromIncludeOptions options;
    options.repeatKey = "maxSimultaneousLights";
    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightUboDeclaration" :
                                                                 "lightFragmentDeclaration",
                                   iComments, options);
  }
  else {
    EmitFunctionFromIncludeOptions options;
    options.replaceStrings = {{R"({X})", std::to_string(_lightId)}};
    state._emitFunctionFromInclude(state.supportUniformBuffers ? "lightUboDeclaration" :
                                                                 "lightFragmentDeclaration",
                                   iComments, options, std::to_string(_lightId));
  }

  state._emitFunctionFromInclude("helperFunctions", iComments);
  state._emitFunctionFromInclude("importanceSampling", iComments);
  state._emitFunctionFromInclude("pbrHelperFunctions", iComments);
  state._emitFunctionFromInclude("imageProcessingFunctions", iComments);

  EmitFunctionFromIncludeOptions functionOptions;
  functionOptions.replaceStrings = {{R"(vPositionW)", worldPosVarName + ".xyz"}};

  state._emitFunctionFromInclude("shadowsFragmentFunctions", iComments, functionOptions);

  state._emitFunctionFromInclude("pbrDirectLightingSetupFunctions", iComments, functionOptions);

  state._emitFunctionFromInclude("pbrDirectLightingFalloffFunctions", iComments);
  state._emitFunctionFromInclude("pbrBRDFFunctions", iComments);
  state._emitFunctionFromInclude("hdrFilteringFunctions", iComments);

  state._emitFunctionFromInclude("pbrDirectLightingFunctions", iComments, functionOptions);

  state._emitFunctionFromInclude("pbrIBLFunctions", iComments);

  state._emitFunctionFromInclude("pbrBlockAlbedoOpacity", iComments);
  state._emitFunctionFromInclude("pbrBlockReflectivity", iComments);
  state._emitFunctionFromInclude("pbrBlockAmbientOcclusion", iComments);
  state._emitFunctionFromInclude("pbrBlockAlphaFresnel", iComments);
  state._emitFunctionFromInclude("pbrBlockAnisotropic", iComments);

  //
  // code
  //

  state._emitUniformFromString("vLightingIntensity", "vec4");

  // _____________________________ Geometry Information ____________________________
  _vNormalWName = state._getFreeVariableName("vNormalW");

  state.compilationString
    += StringTools::printf("vec4 %s = normalize(%s);\r\n", _vNormalWName.c_str(),
                           worldNormal()->associatedVariableName().c_str());

  if (state._registerTempVariable("viewDirectionW")) {
    state.compilationString += StringTools::printf(
      "vec3 viewDirectionW = normalize(%s - %s.xyz);\r\n",
      cameraPosition()->associatedVariableName().c_str(), worldPosVarName.c_str());
  }

  state.compilationString
    += StringTools::printf("vec3 geometricNormalW = %s.xyz;\r\n", _vNormalWName.c_str());

  state.compilationString += StringTools::printf(
    "vec3 normalW = %s;\r\n",
    normalShading->isConnected() ?
      StringTools::printf("normalize(%s.xyz)", normalShading->associatedVariableName().c_str())
        .c_str() :
      "geometricNormalW");

  _invertNormalName = state._getFreeVariableName("invertNormal");

  state._emitUniformFromString(_invertNormalName, "float");

  EmitCodeFromIncludeOptions codeOptions;
  codeOptions.replaceStrings
    = {{R"(vPositionW)", worldPosVarName + ".xyz"}, {R"(vEyePosition.w)", _invertNormalName}};
  state.compilationString
    += state._emitCodeFromInclude("pbrBlockNormalFinal", iComments, codeOptions);

  // _____________________________ Albedo & Opacity ______________________________
  state.compilationString += getAlbedoOpacityCode();

  state.compilationString += state._emitCodeFromInclude("depthPrePass", iComments);

  // _____________________________ AO  _______________________________
  const auto aoBlock = ambientOcc()->connectedPoint() ?
                         std::static_pointer_cast<AmbientOcclusionBlock>(
                           ambientOcc()->connectedPoint()->ownerBlock()) :
                         nullptr;

  state.compilationString += AmbientOcclusionBlock::GetCode(aoBlock);

  state.compilationString += state._emitCodeFromInclude("pbrBlockLightmapInit", iComments);

  // _____________________________ UNLIT  _______________________________

  state.compilationString += R"(#ifdef UNLIT
                    vec3 diffuseBase = vec3(1., 1., 1.);
                #else\r\n
                                       )";

  // _____________________________ Reflectivity _______________________________
  const auto subsurfaceBlock
    = subsurface()->isConnected() && subsurface()->connectedPoint() ?
        std::static_pointer_cast<SubSurfaceBlock>(subsurface()->connectedPoint()->ownerBlock()) :
        nullptr;
  const auto refractionBlock = subsurfaceBlock && subsurfaceBlock->refraction()->connectedPoint() ?
                                 std::static_pointer_cast<RefractionBlock>(
                                   subsurfaceBlock->refraction()->connectedPoint()->ownerBlock()) :
                                 nullptr;

  const auto reflectivityBlock = reflectivity()->connectedPoint() ?
                                   std::static_pointer_cast<ReflectivityBlock>(
                                     reflectivity()->connectedPoint()->ownerBlock()) :
                                   nullptr;

  if (reflectivityBlock) {
    reflectivityBlock->indexOfRefractionConnectionPoint
      = refractionBlock ? refractionBlock->indexOfRefraction() : nullptr;
  }

  const auto aoIntensity = aoBlock ? (aoBlock->intensity()->isConnected() ?
                                        aoBlock->intensity()->associatedVariableName() :
                                        "1.") :
                                     "1.";

  state.compilationString
    += reflectivityBlock ? reflectivityBlock->getCode(state, aoIntensity) : "";

  // _____________________________ Geometry info _________________________________
  codeOptions.replaceStrings
    = {{R"(REFLECTIONMAP_SKYBOX)", reflectionBlock && !reflectionBlock->_defineSkyboxName.empty() ?
                                     reflectionBlock->_defineSkyboxName :
                                     "REFLECTIONMAP_SKYBOX"},
       {R"(REFLECTIONMAP_3D)", reflectionBlock && !reflectionBlock->_define3DName.empty() ?
                                 reflectionBlock->_define3DName :
                                 "REFLECTIONMAP_3D"}};
  state.compilationString
    += state._emitCodeFromInclude("pbrBlockGeometryInfo", iComments, codeOptions);

  // _____________________________ Anisotropy _______________________________________
  const auto anisotropyBlock
    = anisotropy()->isConnected() && anisotropy()->connectedPoint() ?
        std::static_pointer_cast<AnisotropyBlock>(anisotropy()->connectedPoint()->ownerBlock()) :
        nullptr;

  if (anisotropyBlock) {
    anisotropyBlock->worldPositionConnectionPoint = worldPosition();
    anisotropyBlock->worldNormalConnectionPoint   = worldNormal();

    state.compilationString += anisotropyBlock->getCode(state, !perturbedNormal()->isConnected());
  }

  // _____________________________ Reflection _______________________________________
  if (reflectionBlock && reflectionBlock->hasTexture()) {
    state.compilationString += reflectionBlock->getCode(
      state, anisotropyBlock ? "anisotropicOut.anisotropicNormal" : "normalW");
  }

  functionOptions.replaceStrings = {
    {R"(computeReflectionCoords)", "computeReflectionCoordsPBR"},
    {R"(REFLECTIONMAP_3D)", reflectionBlock && !reflectionBlock->_define3DName.empty() ?
                              reflectionBlock->_define3DName :
                              "REFLECTIONMAP_3D"},
    {R"(REFLECTIONMAP_OPPOSITEZ)", reflectionBlock && !reflectionBlock->_defineOppositeZ.empty() ?
                                     reflectionBlock->_defineOppositeZ :
                                     "REFLECTIONMAP_OPPOSITEZ"},
    {R"(REFLECTIONMAP_PROJECTION)",
     reflectionBlock && !reflectionBlock->_defineProjectionName.empty() ?
       reflectionBlock->_defineProjectionName :
       "REFLECTIONMAP_PROJECTION"},
    {R"(REFLECTIONMAP_SKYBOX)", reflectionBlock && !reflectionBlock->_defineSkyboxName.empty() ?
                                  reflectionBlock->_defineSkyboxName :
                                  "REFLECTIONMAP_SKYBOX"},
    {R"(LODINREFLECTIONALPHA)",
     reflectionBlock && !reflectionBlock->_defineLODReflectionAlpha.empty() ?
       reflectionBlock->_defineLODReflectionAlpha :
       "LODINREFLECTIONALPHA"},
    {R"(LINEARSPECULARREFLECTION)",
     reflectionBlock && !reflectionBlock->_defineLinearSpecularReflection.empty() ?
       reflectionBlock->_defineLinearSpecularReflection :
       "LINEARSPECULARREFLECTION"},
    {R"(vReflectionFilteringInfo)",
     reflectionBlock && !reflectionBlock->_vReflectionFilteringInfoName.empty() ?
       reflectionBlock->_vReflectionFilteringInfoName :
       "vReflectionFilteringInfo"},
  };
  state._emitFunctionFromInclude("pbrBlockReflection", iComments, functionOptions);

  // ___________________ Compute Reflectance aka R0 F0 info _________________________
  codeOptions.replaceStrings
    = {{"metallicReflectanceFactors",
        reflectivityBlock && !reflectivityBlock->_vMetallicReflectanceFactorsName.empty() ?
          reflectivityBlock->_vMetallicReflectanceFactorsName :
          "metallicReflectanceFactors"}};
  state.compilationString
    += state._emitCodeFromInclude("pbrBlockReflectance0", iComments, codeOptions);
  // ________________________________ Sheen ______________________________
  const auto sheenBlock
    = sheen()->isConnected() && sheen()->connectedPoint() ?
        std::static_pointer_cast<SheenBlock>(sheen()->connectedPoint()->ownerBlock()) :
        nullptr;

  if (sheenBlock) {
    state.compilationString += sheenBlock->getCode(reflectionBlock);
  }

  functionOptions.replaceStrings = {
    {R"(REFLECTIONMAP_3D)", reflectionBlock && !reflectionBlock->_define3DName.empty() ?
                              reflectionBlock->_define3DName :
                              "REFLECTIONMAP_3D"},
    {R"(REFLECTIONMAP_SKYBOX)", reflectionBlock && !reflectionBlock->_defineSkyboxName.empty() ?
                                  reflectionBlock->_defineSkyboxName :
                                  "REFLECTIONMAP_SKYBOX"},
    {R"(LODINREFLECTIONALPHA)",
     reflectionBlock && !reflectionBlock->_defineLODReflectionAlpha.empty() ?
       reflectionBlock->_defineLODReflectionAlpha :
       "LODINREFLECTIONALPHA"},
    {R"(LINEARSPECULARREFLECTION)",
     reflectionBlock && !reflectionBlock->_defineLinearSpecularReflection.empty() ?
       reflectionBlock->_defineLinearSpecularReflection :
       "LINEARSPECULARREFLECTION"},
  };
  state._emitFunctionFromInclude("pbrBlockSheen", iComments, functionOptions);

  // _____________________________ Clear Coat ____________________________
  const auto clearcoatBlock
    = clearcoat()->isConnected() && clearcoat()->connectedPoint() ?
        std::static_pointer_cast<ClearCoatBlock>(clearcoat()->connectedPoint()->ownerBlock()) :
        nullptr;
  const auto generateTBNSpace = !perturbedNormal()->isConnected() && !anisotropy()->isConnected();
  const auto isTangentConnectedToPerturbNormal
    = perturbedNormal()->isConnected()
      && (perturbedNormal()->connectedPoint()
          && std::static_pointer_cast<PerturbNormalBlock>(
               perturbedNormal()->connectedPoint()->ownerBlock())
               ->worldTangent()
               ->isConnected());
  const auto isTangentConnectedToAnisotropy
    = anisotropy()->isConnected()
      && (anisotropy()->connectedPoint()
          && std::static_pointer_cast<AnisotropyBlock>(anisotropy()->connectedPoint()->ownerBlock())
               ->worldTangent()
               ->isConnected());
  auto vTBNAvailable = isTangentConnectedToPerturbNormal
                       || (!perturbedNormal()->isConnected() && isTangentConnectedToAnisotropy);

  state.compilationString += ClearCoatBlock::GetCode(
    state, clearcoatBlock, reflectionBlock, worldPosVarName, generateTBNSpace, vTBNAvailable,
    worldNormal()->associatedVariableName());

  if (generateTBNSpace) {
    vTBNAvailable = clearcoatBlock ? clearcoatBlock->worldTangent()->isConnected() : false;
  }

  functionOptions.replaceStrings = {
    {R"(computeReflectionCoords)", "computeReflectionCoordsPBR"},
    {R"(REFLECTIONMAP_3D)", reflectionBlock && !reflectionBlock->_define3DName.empty() ?
                              reflectionBlock->_define3DName :
                              "REFLECTIONMAP_3D"},
    {R"(REFLECTIONMAP_OPPOSITEZ)", reflectionBlock && !reflectionBlock->_defineOppositeZ.empty() ?
                                     reflectionBlock->_defineOppositeZ :
                                     "REFLECTIONMAP_OPPOSITEZ"},
    {R"(REFLECTIONMAP_PROJECTION)",
     reflectionBlock && !reflectionBlock->_defineProjectionName.empty() ?
       reflectionBlock->_defineProjectionName :
       "REFLECTIONMAP_PROJECTION"},
    {R"(REFLECTIONMAP_SKYBOX)", reflectionBlock && !reflectionBlock->_defineSkyboxName.empty() ?
                                  reflectionBlock->_defineSkyboxName :
                                  "REFLECTIONMAP_SKYBOX"},
    {R"(LODINREFLECTIONALPHA)",
     reflectionBlock && !reflectionBlock->_defineLODReflectionAlpha.empty() ?
       reflectionBlock->_defineLODReflectionAlpha :
       "LODINREFLECTIONALPHA"},
    {R"(LINEARSPECULARREFLECTION)",
     reflectionBlock && !reflectionBlock->_defineLinearSpecularReflection.empty() ?
       reflectionBlock->_defineLinearSpecularReflection :
       "LINEARSPECULARREFLECTION"},
    {R"(defined\(TANGENT\))", vTBNAvailable ? "defined(TANGENT)" : "defined(IGNORE)"}};
  state._emitFunctionFromInclude("pbrBlockClearcoat", iComments, functionOptions);

  // _________________________ Specular Environment Reflectance __________________________
  codeOptions.replaceStrings = {
    {"REFLECTIONMAP_SKYBOX", reflectionBlock && !reflectionBlock->_defineSkyboxName.empty() ?
                               reflectionBlock->_defineSkyboxName :
                               "REFLECTIONMAP_SKYBOX"},
    {"REFLECTIONMAP_3D", reflectionBlock && !reflectionBlock->_define3DName.empty() ?
                           reflectionBlock->_define3DName :
                           "REFLECTIONMAP_3D"},
  };
  state.compilationString
    += state._emitCodeFromInclude("pbrBlockReflectance", iComments, codeOptions);

  // ___________________________________ SubSurface ______________________________________
  state.compilationString
    += SubSurfaceBlock::GetCode(state, subsurfaceBlock, reflectionBlock, worldPosVarName);

  functionOptions.replaceStrings = {
    {R"(REFLECTIONMAP_3D)", reflectionBlock && !reflectionBlock->_define3DName.empty() ?
                              reflectionBlock->_define3DName :
                              "REFLECTIONMAP_3D"},
    {R"(REFLECTIONMAP_OPPOSITEZ)", reflectionBlock && !reflectionBlock->_defineOppositeZ.empty() ?
                                     reflectionBlock->_defineOppositeZ :
                                     "REFLECTIONMAP_OPPOSITEZ"},
    {R"(REFLECTIONMAP_PROJECTION)",
     reflectionBlock && !reflectionBlock->_defineProjectionName.empty() ?
       reflectionBlock->_defineProjectionName :
       "REFLECTIONMAP_PROJECTION"},
    {R"(SS_REFRACTIONMAP_3D)", refractionBlock && !refractionBlock->_define3DName.empty() ?
                                 refractionBlock->_define3DName :
                                 "SS_REFRACTIONMAP_3D"},
    {R"(SS_LODINREFRACTIONALPHA)",
     refractionBlock && !refractionBlock->_defineLODRefractionAlpha.empty() ?
       refractionBlock->_defineLODRefractionAlpha :
       "SS_LODINREFRACTIONALPHA"},
    {R"(SS_LINEARSPECULARREFRACTION)",
     refractionBlock && !refractionBlock->_defineLinearSpecularRefraction.empty() ?
       refractionBlock->_defineLinearSpecularRefraction :
       "SS_LINEARSPECULARREFRACTION"},
    {R"(SS_REFRACTIONMAP_OPPOSITEZ)",
     refractionBlock && !refractionBlock->_defineOppositeZ.empty() ?
       refractionBlock->_defineOppositeZ :
       "SS_REFRACTIONMAP_OPPOSITEZ"}};
  state._emitFunctionFromInclude("pbrBlockSubSurface", iComments, functionOptions);

  // _____________________________ Direct Lighting Info __________________________________
  state.compilationString += state._emitCodeFromInclude("pbrBlockDirectLighting", iComments);

  if (light) {
    codeOptions.replaceStrings = {{R"({X})", std::to_string(_lightId)}};
    state.compilationString += state._emitCodeFromInclude("lightFragment", iComments, codeOptions);
  }
  else {
    codeOptions.replaceStrings = {};
    codeOptions.repeatKey      = "maxSimultaneousLights";
    state.compilationString += state._emitCodeFromInclude("lightFragment", iComments, codeOptions);
  }

  // _____________________________ Compute Final Lit Components ________________________
  state.compilationString += state._emitCodeFromInclude("pbrBlockFinalLitComponents", iComments);

  // _____________________________ UNLIT (2) ________________________
  state.compilationString += "#endif\r\n"; // UNLIT

  // _____________________________ Compute Final Unlit Components ________________________
  const auto aoColor
    = ambientColor()->isConnected() ? ambientColor()->associatedVariableName() : "vec3(0., 0., 0.)";

  auto aoDirectLightIntensity = aoBlock && aoBlock->directLightIntensity()->isConnected() ?
                                  aoBlock->directLightIntensity()->associatedVariableName() :
                                  std::to_string(PBRBaseMaterial::DEFAULT_AO_ON_ANALYTICAL_LIGHTS);

  if (aoBlock && !aoBlock->directLightIntensity()->isConnected()
      && StringTools::indexOf(aoDirectLightIntensity, ".") == -1) {
    aoDirectLightIntensity += ".";
  }

  codeOptions.replaceStrings = {
    {R"(vec3 finalEmissive[\s\S]*?finalEmissive\*=vLightingIntensity\.y;)", ""},
    {R"(vAmbientColor)", aoColor + " * ambientFromScene"},
    {R"(vAmbientInfos\.w)", aoDirectLightIntensity},
  };
  codeOptions.repeatKey = "";
  state.compilationString
    += state._emitCodeFromInclude("pbrBlockFinalUnlitComponents", iComments, codeOptions);

  // _____________________________ Output Final Color Composition ________________________
  codeOptions.replaceStrings = {{R"(finalEmissive)", "vec3(0.)"}};
  state.compilationString
    += state._emitCodeFromInclude("pbrBlockFinalColorComposition", iComments, codeOptions);

  // _____________________________ Apply image processing ________________________
  codeOptions.replaceStrings = {{R"(visibility)", "1."}};
  state.compilationString
    += state._emitCodeFromInclude("pbrBlockImageProcessing", iComments, codeOptions);

  // _____________________________ Generate debug code ________________________
  codeOptions.replaceStrings
    = {{R"(vNormalW)", _vNormalWName},
       {R"(vPositionW)", worldPosVarName},
       {R"(albedoTexture\.rgb;)",
        "vec3(1.);\r\ngl_FragColor.rgb = toGammaSpace(gl_FragColor.rgb);\r\n"},
       {R"(opacityMap)", opacityTexture()->associatedVariableName()}};
  state.compilationString += state._emitCodeFromInclude("pbrDebug", iComments, codeOptions);

  // _____________________________ Generate end points ________________________
  for (const auto& output : _outputs) {
    if (output->hasEndpoints()) {
      if (mapOutputToVariableContainsName(output->name)) {
        const auto& remap     = mapOutputToVariable.at(output->name);
        const auto varName    = remap.size() > 0 ? remap[0] : "";
        const auto conditions = remap.size() > 1 ? remap[1] : "";
        if (!conditions.empty()) {
          state.compilationString += StringTools::printf("#if %s\r\n", conditions.c_str());
        }
        state.compilationString += StringTools::printf(
          "%s = %s;\r\n", _declareOutput(output, state).c_str(), varName.c_str());
        if (!conditions.empty()) {
          state.compilationString += "#else\r\n";
          state.compilationString
            += StringTools::printf("%s = vec3(0.);\r\n", _declareOutput(output, state).c_str());
          state.compilationString += "#endif\r\n";
        }
      }
      else {
        BABYLON_LOGF_ERROR("PBRMetallicRoughnessBlock",
                           "There's no remapping for the %s end point! No code generated",
                           output->name.c_str());
      }
    }
  }

  return *this;
}

std::string PBRMetallicRoughnessBlock::_dumpPropertiesCode()
{
  std::string codeString;

  codeString
    += StringTools::printf("%s.lightFalloff = %u;\r\n", _codeVariableName.c_str(), lightFalloff);
  codeString
    += StringTools::printf("%s.useAlphaFromAlbedoTexture = %s;\r\n", _codeVariableName.c_str(),
                           useAlphaFromAlbedoTexture ? "true" : "false");
  codeString += StringTools::printf("%s.useAlphaTest = $%s;\r\n", _codeVariableName.c_str(),
                                    useAlphaTest ? "true" : "false");
  codeString += StringTools::printf("%s.alphaTestCutoff = %f;\r\n", _codeVariableName.c_str(),
                                    alphaTestCutoff);
  codeString += StringTools::printf("%s.useAlphaBlending = %s;\r\n", _codeVariableName.c_str(),
                                    useAlphaBlending ? "true" : "false");
  codeString += StringTools::printf("%s.opacityRGB = %s;\r\n", _codeVariableName.c_str(),
                                    opacityRGB ? "true" : "false");
  codeString += StringTools::printf("%s.useRadianceOverAlpha = %s;\r\n", _codeVariableName.c_str(),
                                    useRadianceOverAlpha ? "true" : "false");
  codeString += StringTools::printf("%s.useSpecularOverAlpha = %s;\r\n", _codeVariableName.c_str(),
                                    useSpecularOverAlpha ? "true" : "false");
  codeString
    += StringTools::printf("%s.enableSpecularAntiAliasing = %s;\r\n", _codeVariableName.c_str(),
                           enableSpecularAntiAliasing ? "true" : "false");
  codeString += StringTools::printf("%s.realTimeFiltering = %s;\r\n", _codeVariableName.c_str(),
                                    realTimeFiltering ? "true" : "false");
  codeString += StringTools::printf("%s.realTimeFilteringQuality = %s;\r\n",
                                    _codeVariableName.c_str(), realTimeFilteringQuality);
  codeString += StringTools::printf("%s.useEnergyConservation = %s;\r\n", _codeVariableName.c_str(),
                                    useEnergyConservation ? "true" : "false");
  codeString += StringTools::printf("%s.useRadianceOcclusion = %s;\r\n", _codeVariableName.c_str(),
                                    useRadianceOcclusion ? "true" : "false");
  codeString += StringTools::printf("%s.useHorizonOcclusion = %s;\r\n", _codeVariableName.c_str(),
                                    useHorizonOcclusion ? "true" : "false");
  codeString += StringTools::printf("%s.unlit = %s;\r\n", _codeVariableName.c_str(),
                                    unlit ? "true" : "false");
  codeString += StringTools::printf("%s.forceNormalForward = %s;\r\n", _codeVariableName.c_str(),
                                    forceNormalForward ? "true" : "false");
  codeString += StringTools::printf("%s.debugMode = %u;\r\n", _codeVariableName.c_str(), debugMode);
  codeString
    += StringTools::printf("%s.debugLimit = %f;\r\n", _codeVariableName.c_str(), debugLimit);
  codeString
    += StringTools::printf("%s.debugFactor = %f;\r\n", _codeVariableName.c_str(), debugFactor);

  return codeString;
}

json PBRMetallicRoughnessBlock::serialize() const
{
  return nullptr;
}

void PBRMetallicRoughnessBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                             const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
