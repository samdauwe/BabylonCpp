#include <babylon/materials/material_helper.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/plane.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/morph/morph_target_manager.h>

namespace BABYLON {

void MaterialHelper::BindEyePosition(Effect* effect, Scene* scene)
{
  if (scene->_forcedViewPosition) {
    effect->setVector3("vEyePosition", *scene->_forcedViewPosition.get());
    return;
  }
  effect->setVector3("vEyePosition", scene->_mirroredCameraPosition ?
                                       *scene->_mirroredCameraPosition.get() :
                                       scene->activeCamera->globalPosition());
}

void MaterialHelper::PrepareDefinesForMergedUV(const BaseTexturePtr& texture,
                                               MaterialDefines& defines,
                                               const std::string& key)
{
  defines._needUVs     = true;
  defines.boolDef[key] = true;
  if (texture->getTextureMatrix()->isIdentity(true)) {
    defines.intDef[key + "DIRECTUV"] = texture->coordinatesIndex + 1;
    if (texture->coordinatesIndex == 0) {
      defines.boolDef["MAINUV1"] = true;
    }
    else {
      defines.boolDef["MAINUV1"] = true;
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

  if (!matrix.isIdentity(true)) {
    uniformBuffer.updateMatrix(key + "Matrix", matrix);
  }
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
    defines.boolDef["FOG"]
      = (scene->fogEnabled() && mesh->applyFog()
         && scene->fogMode() != Scene::FOGMODE_NONE() && fogEnabled);
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

  useClipPlane1 = useClipPlane == std::nullopt ? (scene->clipPlane != std::nullopt) :
                                              *useClipPlane;
  useClipPlane2 = useClipPlane == std::nullopt ? (scene->clipPlane2 != std::nullopt) :
                                              *useClipPlane;
  useClipPlane3 = useClipPlane == std::nullopt ? (scene->clipPlane3 != std::nullopt) :
                                              *useClipPlane;
  useClipPlane4 = useClipPlane == std::nullopt ? (scene->clipPlane4 != std::nullopt) :
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
    if (mesh->useBones() && mesh->computeBonesUsingShaders()
        && mesh->skeleton()) {
      defines.intDef["NUM_BONE_INFLUENCERS"] = mesh->numBoneInfluencers();
      defines.intDef["BonesPerMesh"]
        = static_cast<unsigned>(mesh->skeleton()->bones.size() + 1);
    }
    else {
      defines.intDef["NUM_BONE_INFLUENCERS"] = 0;
      defines.intDef["BonesPerMesh"]         = 0;
    }
  }

  if (useMorphTargets) {
    auto _mesh   = static_cast<Mesh*>(mesh);
    auto manager = _mesh ? _mesh->morphTargetManager() : nullptr;
    if (manager) {
      defines.boolDef["MORPHTARGETS_TANGENT"]
        = manager->supportsTangents() && defines["TANGENT"];
      defines.boolDef["MORPHTARGETS_NORMAL"]
        = manager->supportsNormals() && defines["NORMAL"];
      defines.boolDef["MORPHTARGETS"] = (manager->numInfluencers() > 0);
      defines.intDef["NUM_BONE_INFLUENCERS"]
        = static_cast<unsigned>(manager->numInfluencers());
    }
    else {
      defines.boolDef["MORPHTARGETS_TANGENT"] = false;
      defines.boolDef["MORPHTARGETS_NORMAL"]  = false;
      defines.boolDef["MORPHTARGETS"]         = false;
      defines.intDef["NUM_BONE_INFLUENCERS"]  = 0;
    }
  }

  return true;
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

  unsigned int lightIndex = 0;
  bool needNormals        = false;
  bool needRebuild        = false;
  bool lightmapMode       = false;
  bool shadowEnabled      = false;
  bool specularEnabled    = false;

  if (scene->lightsEnabled() && !disableLighting) {
    for (auto& light : mesh->_lightSources) {
      needNormals = true;

      auto lightIndexStr = std::to_string(lightIndex);

      if (!stl_util::contains(defines.boolDef, "LIGHT" + lightIndexStr)) {
        needRebuild = true;
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
        specularEnabled = true;
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
              shadowEnabled = true;
              shadowGenerator->prepareDefines(defines, lightIndex);
            }
          }
        }
      }

      if (light->lightmapMode() != Light::LIGHTMAP_DEFAULT) {
        lightmapMode                                        = true;
        defines.boolDef["LIGHTMAPEXCLUDED" + lightIndexStr] = true;
        defines.boolDef["LIGHTMAPNOSPECULAR" + lightIndexStr]
          = (light->lightmapMode == Light::LIGHTMAP_SHADOWSONLY);
      }
      else {
        defines.boolDef["LIGHTMAPEXCLUDED" + lightIndexStr]   = false;
        defines.boolDef["LIGHTMAPNOSPECULAR" + lightIndexStr] = false;
      }

      ++lightIndex;
      if (lightIndex == maxSimultaneousLights) {
        break;
      }
    }
  }

  defines.boolDef["SPECULARTERM"] = specularEnabled;
  defines.boolDef["SHADOWS"]      = shadowEnabled;

  // Resetting all other lights if any
  auto lightIndexStr = std::to_string(lightIndex);
  for (unsigned int index = lightIndex; index < maxSimultaneousLights;
       ++index) {
    auto indexStr = std::to_string(index);
    if (stl_util::contains(defines.boolDef, "LIGHT" + indexStr)) {
      defines.boolDef["LIGHT" + indexStr]           = false;
      defines.boolDef["HEMILIGHT" + lightIndexStr]  = false;
      defines.boolDef["POINTLIGHT" + lightIndexStr] = false;
      defines.boolDef["DIRLIGHT" + lightIndexStr]   = false;
      defines.boolDef["SPOTLIGHT" + lightIndexStr]  = false;
      defines.boolDef["SHADOW" + lightIndexStr]     = false;
    }
  }

  auto caps = scene->getEngine()->getCaps();

  if (!stl_util::contains(defines.boolDef, "SHADOWFLOAT")) {
    needRebuild = true;
  }

  defines.boolDef["SHADOWFLOAT"]
    = shadowEnabled
      && ((caps.textureFloatRender && caps.textureFloatLinearFiltering)
          || (caps.textureHalfFloatRender
              && caps.textureHalfFloatLinearFiltering));
  defines.boolDef["LIGHTMAPEXCLUDED"] = lightmapMode;

  if (needRebuild) {
    defines.rebuild();
  }

  return needNormals;
}

void MaterialHelper::PrepareUniformsAndSamplersList(
  std::vector<std::string>& uniformsList, std::vector<std::string>& samplersList,
  MaterialDefines& defines, unsigned int maxSimultaneousLights)
{
  std::vector<std::string> uniformBuffersList;

  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    const std::string lightIndexStr = std::to_string(lightIndex);

    if (!stl_util::contains(defines.boolDef, "LIGHT" + lightIndexStr)) {
      break;
    }

    stl_util::concat(uniformsList,
                     {
                       "vLightData" + lightIndexStr,      //
                       "vLightDiffuse" + lightIndexStr,   //
                       "vLightSpecular" + lightIndexStr,  //
                       "vLightDirection" + lightIndexStr, //
                       "vLightFalloff" + lightIndexStr,   //
                       "vLightGround" + lightIndexStr,    //
                       "lightMatrix" + lightIndexStr,     //
                       "shadowsInfo" + lightIndexStr,     //
                       "depthValues" + lightIndexStr      //
                     });

    if (uniformBuffersList.empty()) {
      uniformBuffersList.emplace_back("Light" + lightIndexStr);
    }

    samplersList.emplace_back("shadowSampler" + lightIndexStr);
    samplersList.emplace_back("depthSampler" + lightIndexStr);

    if (stl_util::contains(defines.boolDef,
                           "PROJECTEDLIGHTTEXTURE" + lightIndexStr)
        && defines["PROJECTEDLIGHTTEXTURE" + lightIndexStr]) {
      samplersList.emplace_back("projectionLightSampler" + lightIndexStr);
      uniformsList.emplace_back("textureProjectionMatrix" + lightIndexStr);
    }
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

    stl_util::concat(uniformsList,
                     {
                       "vLightData" + lightIndexStr,      //
                       "vLightDiffuse" + lightIndexStr,   //
                       "vLightSpecular" + lightIndexStr,  //
                       "vLightDirection" + lightIndexStr, //
                       "vLightFalloff" + lightIndexStr,   //
                       "vLightGround" + lightIndexStr,    //
                       "lightMatrix" + lightIndexStr,     //
                       "shadowsInfo" + lightIndexStr,     //
                       "depthValues" + lightIndexStr      //
                     });

    {
      uniformBuffersList.emplace_back("Light" + lightIndexStr);
    }

    samplersList.emplace_back("shadowSampler" + lightIndexStr);
    samplersList.emplace_back("depthSampler" + lightIndexStr);

    if (defines["PROJECTEDLIGHTTEXTURE" + lightIndexStr]) {
      samplersList.emplace_back("projectionLightSampler" + lightIndexStr);
      uniformsList.emplace_back("textureProjectionMatrix" + lightIndexStr);
    }
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

void MaterialHelper::PrepareAttributesForMorphTargets(
  std::vector<std::string>& attribs, AbstractMesh* mesh, MaterialDefines& defines)
{
  auto influencers = defines.intDef["NUM_MORPH_INFLUENCERS"];

  auto engine = Engine::LastCreatedEngine();
  auto _mesh  = static_cast<Mesh*>(mesh);
  if (influencers > 0 && engine && mesh) {
    auto maxAttributesCount
      = static_cast<unsigned>(engine->getCaps().maxVertexAttribs);
    auto manager = _mesh->morphTargetManager();
    auto normal  = manager && manager->supportsNormals() && defines["NORMAL"];
    auto tangent = manager && manager->supportsNormals() && defines["TANGENT"];
    for (unsigned int index = 0; index < influencers; ++index) {
      const auto indexStr = std::to_string(index);
      attribs.emplace_back(std::string(VertexBuffer::PositionKindChars)
                           + indexStr);

      if (normal) {
        attribs.emplace_back(std::string(VertexBuffer::NormalKindChars)
                             + indexStr);
      }

      if (tangent) {
        attribs.emplace_back(std::string(VertexBuffer::TangentKindChars)
                             + indexStr);
      }

      if (attribs.size() > maxAttributesCount) {
        BABYLON_LOGF_ERROR("MaterialHelper",
                           "Cannot add more vertex attributes for mesh %s",
                           mesh->name.c_str());
      }
    }
  }
}

void MaterialHelper::PrepareAttributesForBones(std::vector<std::string>& attribs,
                                               AbstractMesh* mesh,
                                               MaterialDefines& defines,
                                               EffectFallbacks& fallbacks)
{
  if (defines.intDef["NUM_BONE_INFLUENCERS"] > 0) {
    fallbacks.addCPUSkinningFallback(0, mesh);

    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (defines.intDef["NUM_BONE_INFLUENCERS"] > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
  }
}

void MaterialHelper::PrepareAttributesForInstances(std::vector<std::string>& attribs,
                                                   MaterialDefines& defines)
{
  if (defines["INSTANCES"]) {
    attribs.emplace_back(VertexBuffer::World0KindChars);
    attribs.emplace_back(VertexBuffer::World1KindChars);
    attribs.emplace_back(VertexBuffer::World2KindChars);
    attribs.emplace_back(VertexBuffer::World3KindChars);
  }
}

void MaterialHelper::BindLightShadow(Light& light, Scene* /*scene*/,
                                     AbstractMesh& mesh,
                                     unsigned int lightIndex, Effect* effect)
{
  if (light.shadowEnabled && mesh.receiveShadows()) {
    auto shadowGenerator = light.getShadowGenerator();
    if (shadowGenerator) {
      shadowGenerator->bindShadowLight(std::to_string(lightIndex), effect);
    }
  }
}

void MaterialHelper::BindLightProperties(Light& light, Effect* effect,
                                         unsigned int lightIndex)
{
  light.transferToEffect(effect, std::to_string(lightIndex));
}

void MaterialHelper::BindLights(Scene* scene, AbstractMesh* mesh,
                                Effect* effect, MaterialDefines& defines,
                                unsigned int maxSimultaneousLights,
                                bool usePhysicalLightFalloff)
{
  auto len = std::min(mesh->_lightSources.size(),
                        static_cast<size_t>(maxSimultaneousLights));

  for (unsigned int i = 0; i < len; ++i) {

    auto& light    = mesh->_lightSources[i];
    auto iAsString = std::to_string(i);

    auto scaledIntensity = light->getScaledIntensity();
    light->_uniformBuffer->bindToEffect(effect, "Light" + iAsString);

    MaterialHelper::BindLightProperties(*light, effect, i);

    light->diffuse.scaleToRef(scaledIntensity, Tmp::Color3Array[0]);
    light->_uniformBuffer->updateColor4(
      "vLightDiffuse", Tmp::Color3Array[0],
      usePhysicalLightFalloff ? light->radius() : light->range, iAsString);
    if (defines["SPECULARTERM"]) {
      light->specular.scaleToRef(scaledIntensity, Tmp::Color3Array[1]);
      light->_uniformBuffer->updateColor3("vLightSpecular", Tmp::Color3Array[1],
                                          iAsString);
    }

    // Shadows
    if (scene->shadowsEnabled()) {
      BindLightShadow(*light, scene, *mesh, i, effect);
    }
    light->_uniformBuffer->update();
  }
}

void MaterialHelper::BindFogParameters(Scene* scene, AbstractMesh* mesh,
                                       Effect* effect)
{
  if (scene->fogEnabled() && mesh->applyFog()
      && scene->fogMode() != Scene::FOGMODE_NONE()) {
    effect->setFloat4("vFogInfos", static_cast<float>(scene->fogMode()),
                      scene->fogStart, scene->fogEnd, scene->fogDensity);
    effect->setColor3("vFogColor", scene->fogColor);
  }
}

void MaterialHelper::BindBonesParameters(AbstractMesh* mesh, Effect* effect)
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
    const auto& matrices = mesh->skeleton()->getTransformMatrices(mesh);

    if (!matrices.empty()) {
      effect->setMatrices("mBones", matrices);
    }
  }
}

void MaterialHelper::BindMorphTargetParameters(AbstractMesh* abstractMesh,
                                               Effect* effect)
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

void MaterialHelper::BindLogDepth(MaterialDefines& defines, Effect* effect,
                                  Scene* scene)
{
  if (defines["LOGARITHMICDEPTH"]) {
    effect->setFloat(
      "logarithmicDepthConstant",
      2.f / (std::log(scene->activeCamera->maxZ + 1.f) / Math::LN2));
  }
}

void MaterialHelper::BindClipPlane(Effect* effect, Scene* scene)
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
