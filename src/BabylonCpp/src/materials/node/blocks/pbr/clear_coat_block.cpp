#include <babylon/materials/node/blocks/pbr/clear_coat_block.h>

#include <babylon/core/json_util.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/node/blocks/fragment/perturb_normal_block.h>
#include <babylon/materials/node/blocks/input/input_block.h>
#include <babylon/materials/node/blocks/pbr/pbr_metallic_roughness_block.h>
#include <babylon/materials/node/blocks/pbr/reflection_block.h>
#include <babylon/materials/node/node_material_build_state.h>
#include <babylon/materials/node/node_material_build_state_shared_data.h>
#include <babylon/materials/node/node_material_connection_point.h>
#include <babylon/materials/node/node_material_connection_point_custom_object.h>
#include <babylon/materials/node/node_material_defines.h>
#include <babylon/materials/pbr/pbr_clear_coat_configuration.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ClearCoatBlock::ClearCoatBlock(const std::string& iName)
    : NodeMaterialBlock{iName, NodeMaterialBlockTargets::Fragment}
    , remapF0OnInterfaceChange{true}
    , intensity{this, &ClearCoatBlock::get_intensity}
    , roughness{this, &ClearCoatBlock::get_roughness}
    , indexOfRefraction{this, &ClearCoatBlock::get_indexOfRefraction}
    , normalMapColor{this, &ClearCoatBlock::get_normalMapColor}
    , uv{this, &ClearCoatBlock::get_uv}
    , tintColor{this, &ClearCoatBlock::get_tintColor}
    , tintAtDistance{this, &ClearCoatBlock::get_tintAtDistance}
    , tintThickness{this, &ClearCoatBlock::get_tintThickness}
    , worldTangent{this, &ClearCoatBlock::get_worldTangent}
    , clearcoat{this, &ClearCoatBlock::get_clearcoat}
    , _scene{nullptr}
{
  _isUnique = true;
}

ClearCoatBlock::~ClearCoatBlock() = default;

void ClearCoatBlock::RegisterConnections(const ClearCoatBlockPtr& clearCoatBlock)
{
  clearCoatBlock->registerInput("intensity", NodeMaterialBlockConnectionPointTypes::Float, false,
                                NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("roughness", NodeMaterialBlockConnectionPointTypes::Float, true,
                                NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("indexOfRefraction", NodeMaterialBlockConnectionPointTypes::Float,
                                true, NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("normalMapColor", NodeMaterialBlockConnectionPointTypes::Color3,
                                true, NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("uv", NodeMaterialBlockConnectionPointTypes::Vector2, true,
                                NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("tintColor", NodeMaterialBlockConnectionPointTypes::Color3, true,
                                NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("tintAtDistance", NodeMaterialBlockConnectionPointTypes::Float,
                                true, NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("tintThickness", NodeMaterialBlockConnectionPointTypes::Float, true,
                                NodeMaterialBlockTargets::Fragment);
  clearCoatBlock->registerInput("worldTangent", NodeMaterialBlockConnectionPointTypes::Vector4,
                                true);

  clearCoatBlock->registerOutput(
    "clearcoat", NodeMaterialBlockConnectionPointTypes::Object, NodeMaterialBlockTargets::Fragment,
    NodeMaterialConnectionPointCustomObject::New(
      "clearcoat", clearCoatBlock, NodeMaterialConnectionPointDirection::Output,
      [](const std::string& iName) -> ClearCoatBlockPtr { return ClearCoatBlock::New(iName); },
      "ClearCoatBlock"));
}

void ClearCoatBlock::initialize(NodeMaterialBuildState& state)
{
  state._excludeVariableName("clearcoatOut");
  state._excludeVariableName("vClearCoatParams");
  state._excludeVariableName("vClearCoatTintParams");
  state._excludeVariableName("vClearCoatRefractionParams");
  state._excludeVariableName("vClearCoatTangentSpaceParams");
}

std::string ClearCoatBlock::getClassName() const
{
  return "ClearCoatBlock";
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_intensity()
{
  return _inputs[0];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_roughness()
{
  return _inputs[1];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_indexOfRefraction()
{
  return _inputs[2];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_normalMapColor()
{
  return _inputs[3];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_uv()
{
  return _inputs[4];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_tintColor()
{
  return _inputs[5];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_tintAtDistance()
{
  return _inputs[6];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_tintThickness()
{
  return _inputs[7];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_worldTangent()
{
  return _inputs[8];
}

NodeMaterialConnectionPointPtr& ClearCoatBlock::get_clearcoat()
{
  return _outputs[0];
}

void ClearCoatBlock::autoConfigure(const NodeMaterialPtr& /*material*/)
{
  if (!intensity()->isConnected()) {
    auto intensityInput = InputBlock::New("ClearCoat intensity", NodeMaterialBlockTargets::Fragment,
                                          NodeMaterialBlockConnectionPointTypes::Float);
    intensityInput->value = std::make_shared<AnimationValue>(1.f);
    intensityInput->output()->connectTo(intensity());
  }
}

void ClearCoatBlock::prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                                    NodeMaterialDefines& defines, bool /*useInstances*/,
                                    SubMesh* /*subMesh*/)
{
  NodeMaterialBlock::prepareDefines(mesh, nodeMaterial, defines);

  defines.setValue("CLEARCOAT", true);
  defines.setValue("CLEARCOAT_TEXTURE", false, true);
  defines.setValue("CLEARCOAT_USE_ROUGHNESS_FROM_MAINTEXTURE", true, true);
  defines.setValue("CLEARCOAT_TINT",
                   tintColor()->isConnected() || tintThickness()->isConnected()
                     || tintAtDistance()->isConnected(),
                   true);
  defines.setValue("CLEARCOAT_BUMP", normalMapColor()->isConnected(), true);
  defines.setValue("CLEARCOAT_DEFAULTIOR",
                   indexOfRefraction()->isConnected() ?
                     indexOfRefraction()->connectInputBlock()->value()
                       && indexOfRefraction()->connectInputBlock()->value()->get<float>()
                            == PBRClearCoatConfiguration::_DefaultIndexOfRefraction :
                     true,
                   true);
  defines.setValue("CLEARCOAT_REMAP_F0", remapF0OnInterfaceChange, true);
}

void ClearCoatBlock::bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh,
                          SubMesh* /*subMesh*/)
{
  NodeMaterialBlock::bind(effect, nodeMaterial, mesh);

  // Clear Coat Refraction params
  const auto _indexOfRefraction
    = indexOfRefraction()->connectInputBlock()
          && indexOfRefraction()->connectInputBlock()->value() ?
        indexOfRefraction()->connectInputBlock()->value()->get<float>() :
        PBRClearCoatConfiguration::_DefaultIndexOfRefraction;

  const auto a  = 1.f - _indexOfRefraction;
  const auto b  = 1.f + _indexOfRefraction;
  const auto f0 = std::pow((-a / b), 2.f); // Schlicks approx: (ior1 - ior2) / (ior1 + ior2) where
                                           // ior2 for air is close to vacuum = 1.
  const auto eta = 1.f / _indexOfRefraction;

  effect->setFloat4("vClearCoatRefractionParams", f0, eta, a, b);

  // Clear Coat tangent space params
  const PBRMetallicRoughnessBlockPtr mainPBRBlock
    = clearcoat()->hasEndpoints() ? std::static_pointer_cast<PBRMetallicRoughnessBlock>(
        clearcoat()->endpoints()[0]->ownerBlock()) :
                                    nullptr;
  const PerturbNormalBlockPtr perturbedNormalBlock
    = mainPBRBlock && mainPBRBlock->perturbedNormal()->isConnected()
          && mainPBRBlock->perturbedNormal()->connectedPoint() ?
        std::static_pointer_cast<PerturbNormalBlock>(
          mainPBRBlock->perturbedNormal()->connectedPoint()->ownerBlock()) :
        nullptr;

  if (_scene->_mirroredCameraPosition) {
    effect->setFloat2("vClearCoatTangentSpaceParams",
                      perturbedNormalBlock && perturbedNormalBlock->invertX ? 1.f : -1.f,
                      perturbedNormalBlock && perturbedNormalBlock->invertY ? 1.f : -1.f);
  }
  else {
    effect->setFloat2("vClearCoatTangentSpaceParams",
                      perturbedNormalBlock && perturbedNormalBlock->invertX ? -1.f : 1.f,
                      perturbedNormalBlock && perturbedNormalBlock->invertY ? -1.f : 1.f);
  }
}

std::string ClearCoatBlock::_generateTBNSpace(NodeMaterialBuildState& state,
                                              const std::string& /*worldPositionVarName*/,
                                              const std::string& worldNormalVarName)
{
  std::string code = "";

  const auto iComments     = StringTools::printf("//%s", name().c_str());
  const auto iWorldTangent = worldTangent();

  state._emitExtension("derivatives", "#extension GL_OES_standard_derivatives : enable");

  EmitFunctionFromIncludeOptions tangentReplaceStringOptions;
  tangentReplaceStringOptions.replaceStrings
    = {{R"(defined\(TANGENT\)/)",
        iWorldTangent->isConnected() ? "defined(TANGENT)" : "defined(IGNORE)"}};

  if (worldTangent()->isConnected()) {
    code
      += StringTools::printf("vec3 tbnNormal = normalize(%s.xyz);\r\n", worldNormalVarName.c_str());
    code += StringTools::printf("vec3 tbnTangent = normalize(%s.xyz);\r\n",
                                iWorldTangent->associatedVariableName().c_str());
    code += "vec3 tbnBitangent = cross(tbnNormal, tbnTangent);\r\n";
    code += "mat3 vTBN = mat3(tbnTangent, tbnBitangent, tbnNormal);\r\n";
  }

  state._emitFunctionFromInclude("bumpFragmentMainFunctions", comments,
                                 tangentReplaceStringOptions);

  return code;
}

std::string ClearCoatBlock::GetCode(NodeMaterialBuildState& state, const ClearCoatBlockPtr& ccBlock,
                                    const ReflectionBlockPtr& reflectionBlock,
                                    const std::string& worldPosVarName, bool generateTBNSpace,
                                    bool vTBNAvailable, const std::string& worldNormalVarName)
{
  std::string code = "";

  const auto intensity
    = ccBlock->intensity()->isConnected() ? ccBlock->intensity()->associatedVariableName() : "1.";
  const auto roughness
    = ccBlock->roughness()->isConnected() ? ccBlock->roughness()->associatedVariableName() : "0.";
  const auto normalMapColor = ccBlock->normalMapColor()->isConnected() ?
                                ccBlock->normalMapColor()->associatedVariableName() :
                                "vec3(0.)";
  const auto uv
    = ccBlock->uv()->isConnected() ? ccBlock->uv()->associatedVariableName() : "vec2(0.)";

  const auto tintColor      = ccBlock->tintColor()->isConnected() ?
                                ccBlock->tintColor()->associatedVariableName() :
                                "vec3(1.)";
  const auto tintThickness  = ccBlock->tintThickness()->isConnected() ?
                                ccBlock->tintThickness()->associatedVariableName() :
                                "1.";
  const auto tintAtDistance = ccBlock->tintAtDistance()->isConnected() ?
                                ccBlock->tintAtDistance()->associatedVariableName() :
                                "1.";
  const auto tintTexture    = "vec4(0.)";

  if (ccBlock) {
    state._emitUniformFromString("vClearCoatRefractionParams", "vec4");
    state._emitUniformFromString("vClearCoatTangentSpaceParams", "vec2");
  }

  if (generateTBNSpace && ccBlock) {
    code += ccBlock->_generateTBNSpace(state, worldPosVarName, worldNormalVarName);
    vTBNAvailable = ccBlock->worldTangent()->isConnected();
  }

  code += StringTools::printf(
    R"(clearcoatOutParams clearcoatOut;

        #ifdef CLEARCOAT
            vec2 vClearCoatParams = vec2(%s, %s);
            vec4 vClearCoatTintParams = vec4(%s, %s);

            clearcoatBlock(
                %s.xyz,
                geometricNormalW,
                viewDirectionW,
                vClearCoatParams,
                specularEnvironmentR0,
            #ifdef CLEARCOAT_TEXTURE
                vec2(0.),
            #endif
            #ifdef CLEARCOAT_TINT
                vClearCoatTintParams,
                %s,
                vClearCoatRefractionParams,
                #ifdef CLEARCOAT_TINT_TEXTURE
                    %s,
                #endif
            #endif
            #ifdef CLEARCOAT_BUMP
                vec2(0., 1.),
                vec4(%s, 0.),
                %s,
                #if defined(%s) && defined(NORMAL)
                    vTBN,
                #else
                    vClearCoatTangentSpaceParams,
                #endif
                #ifdef OBJECTSPACE_NORMALMAP
                    normalMatrix,
                #endif
            #endif
            #if defined(FORCENORMALFORWARD) && defined(NORMAL)
                faceNormal,
            #endif
            #ifdef REFLECTION
                %s,
                %s,
                %s,
                vLightingIntensity,
                #ifdef %s
                    %s,
                #else
                    %s,
                #endif
                #ifndef LODBASEDMICROSFURACE
                    #ifdef %s
                        %s,
                        %s,
                    #else
                        %s,
                        %s,
                    #endif
                #endif
            #endif
            #if defined(ENVIRONMENTBRDF) && !defined(%s)
                #ifdef RADIANCEOCCLUSION
                    ambientMonochrome,
                #endif
            #endif
            #if defined(CLEARCOAT_BUMP) || defined(TWOSIDEDLIGHTING)
                (gl_FrontFacing ? 1. : -1.),
            #endif
                clearcoatOut
            );
        #else
            clearcoatOut.specularEnvironmentR0 = specularEnvironmentR0;
        #endif\r\n
       )",
    intensity.c_str(), roughness.c_str(),                       //
    tintColor.c_str(), tintThickness.c_str(),                   //
    worldPosVarName.c_str(),                                    //
    tintAtDistance.c_str(),                                     //
    tintTexture,                                                //
    normalMapColor.c_str(),                                     //
    uv.c_str(),                                                 //
    vTBNAvailable ? "TANGENT" : "IGNORE",                       //
    reflectionBlock->_vReflectionMicrosurfaceInfosName.c_str(), //
    reflectionBlock->_vReflectionInfosName.c_str(),             //
    reflectionBlock->reflectionColor().c_str(),                 //
    reflectionBlock->_define3DName.c_str(),                     //
    reflectionBlock->_cubeSamplerName.c_str(),                  //
    reflectionBlock->_2DSamplerName.c_str(),                    //
    reflectionBlock->_define3DName.c_str(),                     //
    reflectionBlock->_cubeSamplerName.c_str(),                  //
    reflectionBlock->_cubeSamplerName.c_str(),                  //
    reflectionBlock->_2DSamplerName.c_str(),                    //
    reflectionBlock->_2DSamplerName.c_str(),                    //
    reflectionBlock->_defineSkyboxName.c_str()                  //
  );

  return code;
}

ClearCoatBlock& ClearCoatBlock::_buildBlock(NodeMaterialBuildState& state)
{
  _scene = state.sharedData->scene;

  if (state.target == NodeMaterialBlockTargets::Fragment) {
    state.sharedData->bindableBlocks.emplace_back(shared_from_this());
    state.sharedData->blocksWithDefines.emplace_back(shared_from_this());
  }

  return *this;
}

std::string ClearCoatBlock::_dumpPropertiesCode()
{
  std::string codeString;

  codeString
    += StringTools::printf("%s.remapF0OnInterfaceChange = %s;\r\n", _codeVariableName.c_str(),
                           remapF0OnInterfaceChange ? "true" : "false");

  return codeString;
}

json ClearCoatBlock::serialize() const
{
  return nullptr;
}

void ClearCoatBlock::_deserialize(const json& /*serializationObject*/, Scene* /*scene*/,
                                  const std::string& /*rootUrl*/)
{
}

} // end of namespace BABYLON
