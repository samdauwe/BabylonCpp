#include <babylon/materials/material_helper.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/plane.h>
#include <babylon/math/tmp.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/morph/morph_target_manager.h>

namespace BABYLON {

std::unique_ptr<MaterialDefines> MaterialHelper::_TmpMorphInfluencers
  = std::make_unique<MaterialDefines>();
Color3 MaterialHelper::_tempFogColor = Color3::Black();

void MaterialHelper::BindEyePosition(const EffectPtr& effect, Scene* scene)
{
  if (scene->_forcedViewPosition) {
    effect->setVector3("vEyePosition", *scene->_forcedViewPosition.get());
    return;
  }
  const auto& globalPosition = scene->activeCamera()->globalPosition();

  effect->setVector3("vEyePosition", scene->_mirroredCameraPosition ?
                                       *scene->_mirroredCameraPosition.get() :
                                       globalPosition);
}

void MaterialHelper::PrepareDefinesForMergedUV(const BaseTexturePtr& texture,
                                               MaterialDefines& defines,
                                               const std::string& key)
{
  defines._needUVs     = true;
  defines.boolDef[key] = true;
  if (texture->getTextureMatrix()->isIdentityAs3x2()) {
    defines.intDef[key + "DIRECTUV"] = texture->coordinatesIndex + 1;
    if (texture->coordinatesIndex == 0) {
      defines.boolDef["MAINUV1"] = true;
    }
    else {
      defines.boolDef["MAINUV2"] = true;
    }
  }
  else {
    defines.intDef[key + "DIRECTUV"] = 0;
  }
}

void MaterialHelper::BindTextureMatrix(BaseTexture& texture,
                                       UniformBuffer& uniformBuffer,
                                       const std::string& key)
{
  auto matrix = *texture.getTextureMatrix();

  uniformBuffer.updateMatrix(key + "Matrix", matrix);
}

bool MaterialHelper::GetFogState(AbstractMesh* mesh, Scene* scene)
{
  return (scene->fogEnabled() && mesh->applyFog()
          && scene->fogMode() != Scene::FOGMODE_NONE);
}

void MaterialHelper::PrepareDefinesForMisc(AbstractMesh* mesh, Scene* scene,
                                           bool useLogarithmicDepth,
                                           bool pointsCloud, bool fogEnabled,
                                           bool alphaTest,
                                           MaterialDefines& defines)
{
  if (defines._areMiscDirty) {
    defines.boolDef["LOGARITHMICDEPTH"] = useLogarithmicDepth;
    defines.boolDef["POINTSIZE"]        = pointsCloud;
    defines.boolDef["FOG"] = fogEnabled && GetFogState(mesh, scene);
    defines.boolDef["NONUNIFORMSCALING"] = mesh->nonUniformScaling();
    defines.boolDef["ALPHATEST"]         = alphaTest;
  }
}

void MaterialHelper::PrepareDefinesForFrameBoundValues(
  Scene* scene, Engine* engine, MaterialDefines& defines, bool useInstances,
  std::optional<bool> useClipPlane)
{
  auto changed       = false;
  auto useClipPlane1 = false;
  auto useClipPlane2 = false;
  auto useClipPlane3 = false;
  auto useClipPlane4 = false;

  useClipPlane1 = useClipPlane == std::nullopt ?
                    (scene->clipPlane != std::nullopt) :
                    *useClipPlane;
  useClipPlane2 = useClipPlane == std::nullopt ?
                    (scene->clipPlane2 != std::nullopt) :
                    *useClipPlane;
  useClipPlane3 = useClipPlane == std::nullopt ?
                    (scene->clipPlane3 != std::nullopt) :
                    *useClipPlane;
  useClipPlane4 = useClipPlane == std::nullopt ?
                    (scene->clipPlane4 != std::nullopt) :
                    *useClipPlane;

  if (defines["CLIPPLANE"] != useClipPlane1) {
    defines.boolDef["CLIPPLANE"] = useClipPlane1;
    changed                      = true;
  }

  if (defines["CLIPPLANE2"] != useClipPlane2) {
    defines.boolDef["CLIPPLANE2"] = useClipPlane2;
    changed                       = true;
  }

  if (defines["CLIPPLANE3"] != useClipPlane3) {
    defines.boolDef["CLIPPLANE3"] = useClipPlane3;
    changed                       = true;
  }

  if (defines["CLIPPLANE4"] != useClipPlane4) {
    defines.boolDef["CLIPPLANE4"] = useClipPlane4;
    changed                       = true;
  }

  if (defines["DEPTHPREPASS"] != !engine->getColorWrite()) {
    defines.boolDef["DEPTHPREPASS"] = !defines["DEPTHPREPASS"];
    changed                         = true;
  }

  if (defines["INSTANCES"] != useInstances) {
    defines.boolDef["INSTANCES"] = useInstances;
    changed                      = true;
  }

  if (changed) {
    defines.markAsUnprocessed();
  }
}

void MaterialHelper::PrepareDefinesForBones(AbstractMesh* mesh,
                                            MaterialDefines& defines)
{
  if (mesh->useBones() && mesh->computeBonesUsingShaders()
      && mesh->skeleton()) {
    defines.intDef["NUM_BONE_INFLUENCERS"] = mesh->numBoneInfluencers();

    const auto materialSupportsBoneTexture
      = stl_util::contains(defines.boolDef, "BONETEXTURE");

    if (mesh->skeleton()->isUsingTextureForMatrices
        && materialSupportsBoneTexture) {
      defines.boolDef["BONETEXTURE"] = true;
    }
    else {
      defines.intDef["BonesPerMesh"]
        = static_cast<unsigned int>(mesh->skeleton()->bones.size() + 1);
      if (materialSupportsBoneTexture) {
        defines.boolDef["BONETEXTURE"] = false;
      }
      else {
        defines.boolDef.erase("BONETEXTURE");
      }
    }
  }
  else {
    defines.intDef["NUM_BONE_INFLUENCERS"] = 0;
    defines.intDef["BonesPerMesh"]         = 0;
  }
}

void MaterialHelper::PrepareDefinesForMorphTargets(AbstractMesh* mesh,
                                                   MaterialDefines& defines)
{
  const auto& manager = static_cast<Mesh*>(mesh)->morphTargetManager();
  if (manager) {
    defines.boolDef["MORPHTARGETS_UV"]
      = manager->supportsUVs() && defines["UV1"];
    defines.boolDef["MORPHTARGETS_TANGENT"]
      = manager->supportsTangents() && defines["TANGENT"];
    defines.boolDef["MORPHTARGETS_NORMAL"]
      = manager->supportsNormals() && defines["NORMAL"];
    defines.boolDef["MORPHTARGETS"] = (manager->numInfluencers() > 0);
    defines.intDef["NUM_MORPH_INFLUENCERS"]
      = static_cast<unsigned int>(manager->numInfluencers());
  }
  else {
    defines.boolDef["MORPHTARGETS_UV"]      = false;
    defines.boolDef["MORPHTARGETS_TANGENT"] = false;
    defines.boolDef["MORPHTARGETS_NORMAL"]  = false;
    defines.boolDef["MORPHTARGETS"]         = false;
    defines.intDef["NUM_MORPH_INFLUENCERS"] = 0u;
  }
}

bool MaterialHelper::PrepareDefinesForAttributes(
  AbstractMesh* mesh, MaterialDefines& defines, bool useVertexColor,
  bool useBones, bool useMorphTargets, bool useVertexAlpha)
{
  if (!defines._areAttributesDirty && defines._needNormals == defines._normals
      && defines._needUVs == defines._uvs) {
    return false;
  }

  defines._normals = defines._needNormals;
  defines._uvs     = defines._needUVs;

  defines.boolDef["NORMAL"]
    = (defines._needNormals
       && mesh->isVerticesDataPresent(VertexBuffer::NormalKind));

  if (defines._needNormals
      && mesh->isVerticesDataPresent(VertexBuffer::TangentKind)) {
    defines.boolDef["TANGENT"] = true;
  }

  if (defines._needUVs) {
    defines.boolDef["UV1"] = mesh->isVerticesDataPresent(VertexBuffer::UVKind);
    defines.boolDef["UV2"] = mesh->isVerticesDataPresent(VertexBuffer::UV2Kind);
  }
  else {
    defines.boolDef["UV1"] = false;
    defines.boolDef["UV2"] = false;
  }

  if (useVertexColor) {
    auto hasVertexColors
      = mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind);
    defines.boolDef["VERTEXCOLOR"] = hasVertexColors;
    defines.boolDef["VERTEXALPHA"]
      = mesh->hasVertexAlpha() && hasVertexColors && useVertexAlpha;
  }

  if (useBones) {
    PrepareDefinesForBones(mesh, defines);
  }

  if (useMorphTargets) {
    PrepareDefinesForMorphTargets(mesh, defines);
  }

  return true;
}

void MaterialHelper::PrepareDefinesForMultiview(Scene* scene,
                                                MaterialDefines& defines)
{
  if (scene->activeCamera()) {
    const auto previousMultiview = defines["MULTIVIEW"];
    defines.boolDef["MULTIVIEW"]
      = (scene->activeCamera()->outputRenderTarget != nullptr
         && scene->activeCamera()->outputRenderTarget->getViewCount() > 1);
    if (defines["MULTIVIEW"] != previousMultiview) {
      defines.markAsUnprocessed();
    }
  }
}

void MaterialHelper::PrepareDefinesForLight(Scene* scene, AbstractMesh* mesh,
                                            const LightPtr& light,
                                            unsigned int lightIndex,
                                            MaterialDefines& defines,
                                            bool specularSupported,
                                            PrepareDefinesForLightsState& state)
{
  state.needNormals = true;

  auto lightIndexStr = std::to_string(lightIndex);

  if (!stl_util::contains(defines.boolDef, "LIGHT" + lightIndexStr)) {
    state.needRebuild = true;
  }

  defines.boolDef["LIGHT" + lightIndexStr] = true;

  defines.boolDef["SPOTLIGHT" + lightIndexStr]  = false;
  defines.boolDef["HEMILIGHT" + lightIndexStr]  = false;
  defines.boolDef["POINTLIGHT" + lightIndexStr] = false;
  defines.boolDef["DIRLIGHT" + lightIndexStr]   = false;

  light->prepareLightSpecificDefines(defines, lightIndex);

  // FallOff.
  defines.boolDef["LIGHT_FALLOFF_PHYSICAL" + lightIndexStr] = false;
  defines.boolDef["LIGHT_FALLOFF_GLTF" + lightIndexStr]     = false;
  defines.boolDef["LIGHT_FALLOFF_STANDARD" + lightIndexStr] = false;

  switch (light->falloffType) {
    case Light::FALLOFF_GLTF:
      defines.boolDef["LIGHT_FALLOFF_GLTF" + lightIndexStr] = true;
      break;
    case Light::FALLOFF_PHYSICAL:
      defines.boolDef["LIGHT_FALLOFF_PHYSICAL" + lightIndexStr] = true;
      break;
    case Light::FALLOFF_STANDARD:
      defines.boolDef["LIGHT_FALLOFF_STANDARD" + lightIndexStr] = true;
      break;
  }

  // Specular
  if (specularSupported && !light->specular.equalsFloats(0.f, 0.f, 0.f)) {
    state.specularEnabled = true;
  }

  // Shadows
  defines.boolDef["SHADOW" + lightIndexStr]              = false;
  defines.boolDef["SHADOWPCF" + lightIndexStr]           = false;
  defines.boolDef["SHADOWPCSS" + lightIndexStr]          = false;
  defines.boolDef["SHADOWPOISSON" + lightIndexStr]       = false;
  defines.boolDef["SHADOWESM" + lightIndexStr]           = false;
  defines.boolDef["SHADOWCUBE" + lightIndexStr]          = false;
  defines.boolDef["SHADOWLOWQUALITY" + lightIndexStr]    = false;
  defines.boolDef["SHADOWMEDIUMQUALITY" + lightIndexStr] = false;

  if (mesh && mesh->receiveShadows() && scene->shadowsEnabled()
      && light->shadowEnabled) {
    const auto& shadowGenerator = light->getShadowGenerator();
    if (shadowGenerator) {
      const auto& shadowMap = shadowGenerator->getShadowMap();
      if (shadowMap) {
        if (!shadowMap->renderList().empty()) {
          state.shadowEnabled = true;
          shadowGenerator->prepareDefines(defines, lightIndex);
        }
      }
    }
  }

  if (light->lightmapMode() != Light::LIGHTMAP_DEFAULT) {
    state.lightmapMode                                  = true;
    defines.boolDef["LIGHTMAPEXCLUDED" + lightIndexStr] = true;
    defines.boolDef["LIGHTMAPNOSPECULAR" + lightIndexStr]
      = (light->lightmapMode == Light::LIGHTMAP_SHADOWSONLY);
  }
  else {
    defines.boolDef["LIGHTMAPEXCLUDED" + lightIndexStr]   = false;
    defines.boolDef["LIGHTMAPNOSPECULAR" + lightIndexStr] = false;
  }
}

bool MaterialHelper::PrepareDefinesForLights(Scene* scene, AbstractMesh* mesh,
                                             MaterialDefines& defines,
                                             bool specularSupported,
                                             unsigned int maxSimultaneousLights,
                                             bool disableLighting)
{
  if (!defines._areLightsDirty) {
    return defines._needNormals;
  }

  auto lightIndex = 0u;
  PrepareDefinesForLightsState state{
    false, // needNormals
    false, // needRebuild
    false, // shadowEnabled
    false, // specularEnabled
    false  // lightmapMode
  };

  if (scene->lightsEnabled() && !disableLighting) {
    for (const auto& light : mesh->lightSources()) {
      PrepareDefinesForLight(scene, mesh, light, lightIndex, defines,
                             specularSupported, state);

      ++lightIndex;
      if (lightIndex == maxSimultaneousLights) {
        break;
      }
    }
  }

  defines.boolDef["SPECULARTERM"] = state.specularEnabled;
  defines.boolDef["SHADOWS"]      = state.shadowEnabled;

  // Resetting all other lights if any
  auto lightIndexStr = std::to_string(lightIndex);
  for (auto index = lightIndex; index < maxSimultaneousLights; ++index) {
    const auto indexStr = std::to_string(index);
    if (stl_util::contains(defines.boolDef, "LIGHT" + indexStr)) {
      defines.boolDef["LIGHT" + indexStr]               = false;
      defines.boolDef["HEMILIGHT" + indexStr]           = false;
      defines.boolDef["POINTLIGHT" + indexStr]          = false;
      defines.boolDef["DIRLIGHT" + indexStr]            = false;
      defines.boolDef["SPOTLIGHT" + indexStr]           = false;
      defines.boolDef["SHADOW" + indexStr]              = false;
      defines.boolDef["SHADOWPCF" + indexStr]           = false;
      defines.boolDef["SHADOWPCSS" + indexStr]          = false;
      defines.boolDef["SHADOWPOISSON" + indexStr]       = false;
      defines.boolDef["SHADOWESM" + indexStr]           = false;
      defines.boolDef["SHADOWCUBE" + indexStr]          = false;
      defines.boolDef["SHADOWLOWQUALITY" + indexStr]    = false;
      defines.boolDef["SHADOWMEDIUMQUALITY" + indexStr] = false;
    }
  }

  auto caps = scene->getEngine()->getCaps();

  if (!stl_util::contains(defines.boolDef, "SHADOWFLOAT")) {
    state.needRebuild = true;
  }

  defines.boolDef["SHADOWFLOAT"]
    = state.shadowEnabled
      && ((caps.textureFloatRender && caps.textureFloatLinearFiltering)
          || (caps.textureHalfFloatRender
              && caps.textureHalfFloatLinearFiltering));
  defines.boolDef["LIGHTMAPEXCLUDED"] = state.lightmapMode;

  if (state.needRebuild) {
    defines.rebuild();
  }

  return state.needNormals;
}

void MaterialHelper::PrepareUniformsAndSamplersForLight(
  unsigned int lightIndex, std::vector<std::string>& uniformsList,
  std::vector<std::string>& samplersList, bool projectedLightTexture)
{

  std::vector<std::string> uniformBuffersList;
  PrepareUniformsAndSamplersForLight(lightIndex, uniformsList, samplersList,
                                     uniformBuffersList, false,
                                     projectedLightTexture);
}

void MaterialHelper::PrepareUniformsAndSamplersForLight(
  unsigned int lightIndex, std::vector<std::string>& uniformsList,
  std::vector<std::string>& samplersList,
  std::vector<std::string>& uniformBuffersList, bool hasUniformBuffersList,
  bool projectedLightTexture)
{
  const auto lightIndexStr = std::to_string(lightIndex);
  stl_util::concat(uniformsList, {
                                   "vLightData" + lightIndexStr,      //
                                   "vLightDiffuse" + lightIndexStr,   //
                                   "vLightSpecular" + lightIndexStr,  //
                                   "vLightDirection" + lightIndexStr, //
                                   "vLightFalloff" + lightIndexStr,   //
                                   "vLightGround" + lightIndexStr,    //
                                   "lightMatrix" + lightIndexStr,     //
                                   "shadowsInfo" + lightIndexStr,     //
                                   "depthValues" + lightIndexStr,     //
                                 });

  if (hasUniformBuffersList) {
    uniformBuffersList.emplace_back("Light" + lightIndexStr);
  }

  samplersList.emplace_back("shadowSampler" + lightIndexStr);
  samplersList.emplace_back("depthSampler" + lightIndexStr);

  if (projectedLightTexture) {
    samplersList.emplace_back("projectionLightSampler" + lightIndexStr);
    uniformsList.emplace_back("textureProjectionMatrix" + lightIndexStr);
  }
}

void MaterialHelper::PrepareUniformsAndSamplersList(
  std::vector<std::string>& uniformsList,
  std::vector<std::string>& samplersList, MaterialDefines& defines,
  unsigned int maxSimultaneousLights)
{
  std::vector<std::string> uniformBuffersList;

  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    const auto lightIndexStr = std::to_string(lightIndex);

    if (!defines["LIGHT" + lightIndexStr]) {
      break;
    }
    PrepareUniformsAndSamplersForLight(
      lightIndex, uniformsList, samplersList, uniformBuffersList,
      !uniformBuffersList.empty(),
      defines["PROJECTEDLIGHTTEXTURE" + lightIndexStr]);
  }

  if (stl_util::contains(defines.intDef, "NUM_MORPH_INFLUENCERS")
      && defines.intDef["NUM_MORPH_INFLUENCERS"]) {
    uniformsList.emplace_back("morphTargetInfluences");
  }
}

void MaterialHelper::PrepareUniformsAndSamplersList(
  EffectCreationOptions& options)
{
  auto& uniformsList          = options.uniformsNames;
  auto& uniformBuffersList    = options.uniformBuffersNames;
  auto& samplersList          = options.samplers;
  auto& defines               = *options.materialDefines;
  auto& maxSimultaneousLights = options.maxSimultaneousLights;

  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    const std::string lightIndexStr = std::to_string(lightIndex);

    if (!defines["LIGHT" + lightIndexStr]) {
      break;
    }
    PrepareUniformsAndSamplersForLight(
      lightIndex, uniformsList, samplersList, uniformBuffersList,
      !uniformBuffersList.empty(),
      defines["PROJECTEDLIGHTTEXTURE" + lightIndexStr]);
  }

  if (stl_util::contains(defines.intDef, "NUM_MORPH_INFLUENCERS")
      && defines.intDef["NUM_MORPH_INFLUENCERS"]) {
    uniformsList.emplace_back("morphTargetInfluences");
  }
}

unsigned int MaterialHelper::HandleFallbacksForShadows(
  MaterialDefines& defines, EffectFallbacks& fallbacks,
  unsigned int maxSimultaneousLights, unsigned int rank)
{
  unsigned int lightFallbackRank = 0;
  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    const std::string lightIndexStr = std::to_string(lightIndex);

    if (!stl_util::contains(defines.boolDef, "LIGHT" + lightIndexStr)) {
      break;
    }

    if (lightIndex > 0) {
      lightFallbackRank = rank + lightIndex;
      fallbacks.addFallback(lightFallbackRank, "LIGHT" + lightIndexStr);
    }

    if (!defines["SHADOWS"]) {
      if (defines["SHADOW" + lightIndexStr]) {
        fallbacks.addFallback(rank, "SHADOW" + lightIndexStr);
      }

      if (defines["SHADOWPCF" + lightIndexStr]) {
        fallbacks.addFallback(rank, "SHADOWPCF" + lightIndexStr);
      }

      if (defines["SHADOWPCSS" + lightIndexStr]) {
        fallbacks.addFallback(rank, "SHADOWPCSS" + lightIndexStr);
      }

      if (defines["SHADOWPOISSON" + lightIndexStr]) {
        fallbacks.addFallback(rank, "SHADOWPOISSON" + lightIndexStr);
      }

      if (defines["SHADOWESM" + lightIndexStr]) {
        fallbacks.addFallback(rank, "SHADOWESM" + lightIndexStr);
      }
    }
  }
  return lightFallbackRank++;
}

void MaterialHelper::PrepareAttributesForMorphTargetsInfluencers(
  std::vector<std::string>& attribs, AbstractMesh* mesh,
  unsigned int influencers)
{
  _TmpMorphInfluencers->intDef["NUM_MORPH_INFLUENCERS"] = influencers;
  PrepareAttributesForMorphTargets(attribs, mesh, *_TmpMorphInfluencers);
}

void MaterialHelper::PrepareAttributesForMorphTargets(
  std::vector<std::string>& attribs, AbstractMesh* mesh,
  MaterialDefines& defines)
{
  auto influencers = defines.intDef["NUM_MORPH_INFLUENCERS"];

  auto engine = Engine::LastCreatedEngine();
  auto _mesh  = static_cast<Mesh*>(mesh);
  if (influencers > 0 && engine && _mesh) {
    auto maxAttributesCount
      = static_cast<unsigned>(engine->getCaps().maxVertexAttribs);
    auto manager = _mesh->morphTargetManager();
    auto normal  = manager && manager->supportsNormals() && defines["NORMAL"];
    auto tangent = manager && manager->supportsNormals() && defines["TANGENT"];
    auto uv      = manager && manager->supportsUVs() && defines["UV1"];
    for (auto index = 0u; index < influencers; ++index) {
      const auto indexStr = std::to_string(index);
      attribs.emplace_back(VertexBuffer::PositionKind + indexStr);

      if (normal) {
        attribs.emplace_back(VertexBuffer::NormalKind + indexStr);
      }

      if (tangent) {
        attribs.emplace_back(VertexBuffer::TangentKind + indexStr);
      }

      if (uv) {
        attribs.emplace_back(VertexBuffer::UVKind + std::string("_")
                             + indexStr);
      }

      if (attribs.size() > maxAttributesCount) {
        BABYLON_LOGF_ERROR("MaterialHelper",
                           "Cannot add more vertex attributes for mesh %s",
                           mesh->name.c_str())
      }
    }
  }
}

void MaterialHelper::PrepareAttributesForBones(
  std::vector<std::string>& attribs, AbstractMesh* mesh,
  MaterialDefines& defines, EffectFallbacks& fallbacks)
{
  if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
    fallbacks.addCPUSkinningFallback(0, mesh);

    attribs.emplace_back(VertexBuffer::MatricesIndicesKind);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKind);
    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKind);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKind);
    }
  }
}

void MaterialHelper::PrepareAttributesForInstances(
  std::vector<std::string>& attribs, MaterialDefines& defines)
{
  if (defines["INSTANCES"]) {
    PushAttributesForInstances(attribs);
  }
}

void MaterialHelper::PushAttributesForInstances(
  std::vector<std::string>& attribs)
{
  attribs.emplace_back(VertexBuffer::World0Kind);
  attribs.emplace_back(VertexBuffer::World1Kind);
  attribs.emplace_back(VertexBuffer::World2Kind);
  attribs.emplace_back(VertexBuffer::World3Kind);
}

void MaterialHelper::BindLightShadow(Light& light, AbstractMesh& mesh,
                                     const std::string& lightIndex,
                                     const EffectPtr& effect)
{
  if (light.shadowEnabled && mesh.receiveShadows()) {
    auto shadowGenerator = light.getShadowGenerator();
    if (shadowGenerator) {
      shadowGenerator->bindShadowLight(lightIndex, effect);
    }
  }
}

void MaterialHelper::BindLightProperties(Light& light, const EffectPtr& effect,
                                         unsigned int lightIndex)
{
  light.transferToEffect(effect, std::to_string(lightIndex));
}

void MaterialHelper::BindLight(const LightPtr& light, unsigned int lightIndex,
                               Scene* scene, AbstractMesh* mesh,
                               const EffectPtr& effect, bool useSpecular,
                               bool usePhysicalLightFalloff,
                               bool rebuildInParallel)
{
  auto iAsString = std::to_string(lightIndex);

  auto scaledIntensity = light->getScaledIntensity();
  if (!rebuildInParallel) {
    light->_uniformBuffer->bindToEffect(effect.get(), "Light" + iAsString);
  }

  MaterialHelper::BindLightProperties(*light, effect, lightIndex);

  light->diffuse.scaleToRef(scaledIntensity, Tmp::Color3Array[0]);
  light->_uniformBuffer->updateColor4(
    "vLightDiffuse", Tmp::Color3Array[0],
    usePhysicalLightFalloff ? light->radius() : light->range, iAsString);
  if (useSpecular) {
    light->specular.scaleToRef(scaledIntensity, Tmp::Color3Array[1]);
    light->_uniformBuffer->updateColor3("vLightSpecular", Tmp::Color3Array[1],
                                        iAsString);
  }

  // Shadows
  if (scene->shadowsEnabled()) {
    BindLightShadow(*light, *mesh, iAsString, effect);
  }
  light->_uniformBuffer->update();
}

void MaterialHelper::BindLights(Scene* scene, AbstractMesh* mesh,
                                const EffectPtr& effect,
                                MaterialDefines& defines,
                                unsigned int maxSimultaneousLights,
                                bool usePhysicalLightFalloff,
                                bool rebuildInParallel)
{
  auto len = std::min(mesh->lightSources().size(),
                      static_cast<size_t>(maxSimultaneousLights));

  for (unsigned i = 0u; i < len; ++i) {

    auto& light = mesh->lightSources()[i];
    BindLight(light, i, scene, mesh, effect, defines["SPECULARTERM"],
              usePhysicalLightFalloff, rebuildInParallel);
  }
}

void MaterialHelper::BindFogParameters(Scene* scene, AbstractMesh* mesh,
                                       const EffectPtr& effect,
                                       bool linearSpace)
{
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE) {
    effect->setFloat4("vFogInfos", static_cast<float>(scene->fogMode()),
                      scene->fogStart, scene->fogEnd, scene->fogDensity);
    // Convert fog color to linear space if used in a linear space computed
    // shader.
    if (linearSpace) {
      scene->fogColor.toLinearSpaceToRef(MaterialHelper::_tempFogColor);
      effect->setColor3("vFogColor", MaterialHelper::_tempFogColor);
    }
    else {
      effect->setColor3("vFogColor", scene->fogColor);
    }
  }
}

void MaterialHelper::BindBonesParameters(AbstractMesh* mesh,
                                         const EffectPtr& effect)
{
  if (!effect || !mesh) {
    return;
  }
  if (mesh->computeBonesUsingShaders()
      && effect->_bonesComputationForcedToCPU) {
    mesh->computeBonesUsingShaders = false;
  }

  if (mesh->useBones() && mesh->computeBonesUsingShaders()
      && mesh->skeleton()) {
    const auto& skeleton = mesh->skeleton();

    if (skeleton->isUsingTextureForMatrices
        && effect->getUniformIndex("boneTextureWidth") > -1) {
      const auto& boneTexture = skeleton->getTransformMatrixTexture(mesh);
      effect->setTexture("boneSampler", boneTexture);
      effect->setFloat("boneTextureWidth",
                       4.f * (skeleton->bones.size() + 1.f));
    }
    else {
      const auto& matrices = skeleton->getTransformMatrices(mesh);

      if (!matrices.empty()) {
        effect->setMatrices("mBones", matrices);
      }
    }
  }
}

void MaterialHelper::BindMorphTargetParameters(AbstractMesh* abstractMesh,
                                               const EffectPtr& effect)
{
  auto mesh = static_cast<Mesh*>(abstractMesh);
  if (mesh) {
    auto manager = mesh->morphTargetManager();
    if (!abstractMesh || !manager) {
      return;
    }

    effect->setFloatArray("morphTargetInfluences", manager->influences());
  }
}

void MaterialHelper::BindLogDepth(MaterialDefines& defines,
                                  const EffectPtr& effect, Scene* scene)
{
  if (defines["LOGARITHMICDEPTH"]) {
    effect->setFloat(
      "logarithmicDepthConstant",
      2.f / (std::log(scene->activeCamera()->maxZ + 1.f) / Math::LN2));
  }
}

void MaterialHelper::BindClipPlane(const EffectPtr& effect, Scene* scene)
{
  if (scene->clipPlane) {
    const auto& clipPlane = *scene->clipPlane;
    effect->setFloat4("vClipPlane", clipPlane.normal.x, clipPlane.normal.y,
                      clipPlane.normal.z, clipPlane.d);
  }
  if (scene->clipPlane2) {
    const auto& clipPlane = *scene->clipPlane2;
    effect->setFloat4("vClipPlane2", clipPlane.normal.x, clipPlane.normal.y,
                      clipPlane.normal.z, clipPlane.d);
  }
  if (scene->clipPlane3) {
    const auto& clipPlane = *scene->clipPlane3;
    effect->setFloat4("vClipPlane3", clipPlane.normal.x, clipPlane.normal.y,
                      clipPlane.normal.z, clipPlane.d);
  }
  if (scene->clipPlane4) {
    const auto& clipPlane = *scene->clipPlane4;
    effect->setFloat4("vClipPlane4", clipPlane.normal.x, clipPlane.normal.y,
                      clipPlane.normal.z, clipPlane.d);
  }
}

} // end of namespace BABYLON
