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

void MaterialHelper::PrepareDefinesForMergedUV(
  BaseTexture* texture, MaterialDefines& defines, unsigned int key,
  const string_t& keyString, unsigned int MAINUV1, unsigned int MAINUV2)
{
  const auto updateDirectUV = [&defines, &keyString](unsigned int value) {
    if (keyString == "DIFFUSE") {
      defines.DIFFUSEDIRECTUV = value;
    }
    else if (keyString == "AMBIENT") {
      defines.AMBIENTDIRECTUV = value;
    }
    else if (keyString == "OPACITY") {
      defines.OPACITYDIRECTUV = value;
    }
    else if (keyString == "EMISSIVE") {
      defines.EMISSIVEDIRECTUV = value;
    }
    else if (keyString == "SPECULAR") {
      defines.SPECULARDIRECTUV = value;
    }
    else if (keyString == "BUMP") {
      defines.BUMPDIRECTUV = value;
    }
    else if (keyString == "LIGHTMAP") {
      defines.LIGHTMAPDIRECTUV = value;
    }
    else if (keyString == "ALBEDO") {
      defines.ALBEDODIRECTUV = value;
    }
    else if (keyString == "REFLECTIVITY") {
      defines.REFLECTIVITYDIRECTUV = value;
    }
    else if (keyString == "MICROSURFACEMAP") {
      defines.MICROSURFACEMAPDIRECTUV = value;
    }
  };

  defines._needUVs     = true;
  defines.defines[key] = true;
  if (texture->getTextureMatrix()->isIdentity(true)) {
    updateDirectUV(texture->coordinatesIndex + 1);
    if (texture->coordinatesIndex == 0) {
      defines.defines[MAINUV1] = true;
    }
    else {
      defines.defines[MAINUV2] = true;
    }
  }
  else {
    updateDirectUV(0);
  }
}

void MaterialHelper::BindTextureMatrix(BaseTexture& texture,
                                       UniformBuffer& uniformBuffer,
                                       const string_t& key)
{
  auto matrix = *texture.getTextureMatrix();

  if (!matrix.isIdentity(true)) {
    uniformBuffer.updateMatrix(key + "Matrix", matrix);
  }
}

void MaterialHelper::PrepareDefinesForMisc(
  AbstractMesh* mesh, Scene* scene, bool useLogarithmicDepth, bool pointsCloud,
  bool fogEnabled, bool alphaTest, MaterialDefines& defines,
  unsigned int LOGARITHMICDEPTH, unsigned int POINTSIZE, unsigned int FOG,
  unsigned int NONUNIFORMSCALING, unsigned int ALPHATEST)
{
  if (defines._areMiscDirty) {
    defines.defines[LOGARITHMICDEPTH] = useLogarithmicDepth;
    defines.defines[POINTSIZE]        = pointsCloud;
    defines.defines[FOG]
      = (scene->fogEnabled() && mesh->applyFog()
         && scene->fogMode() != Scene::FOGMODE_NONE() && fogEnabled);
    defines.defines[NONUNIFORMSCALING] = mesh->nonUniformScaling();
    defines.defines[ALPHATEST]         = alphaTest;
  }
}

void MaterialHelper::PrepareDefinesForFrameBoundValues(
  Scene* scene, Engine* engine, MaterialDefines& defines, bool useInstances,
  unsigned int CLIPPLANE, unsigned int DEPTHPREPASS, unsigned int INSTANCES,
  Nullable<bool> useClipPlane)
{
  bool changed = false;

  if (useClipPlane == nullptr) {
    useClipPlane = (scene->clipPlane() != nullptr);
  }

  if (defines[CLIPPLANE] != *useClipPlane) {
    defines.defines[CLIPPLANE] = *useClipPlane;
    changed                    = true;
  }

  if (defines[DEPTHPREPASS] != !engine->getColorWrite()) {
    defines.defines[DEPTHPREPASS] = !defines[DEPTHPREPASS];
    changed                       = true;
  }

  if (defines[INSTANCES] != useInstances) {
    defines.defines[INSTANCES] = useInstances;
    changed                    = true;
  }

  if (changed) {
    defines.markAsUnprocessed();
  }
}

bool MaterialHelper::PrepareDefinesForAttributes(
  AbstractMesh* mesh, MaterialDefines& defines, bool useVertexColor,
  bool useBones, bool useMorphTargets, bool useVertexAlpha, unsigned int NORMAL,
  unsigned int UV1, unsigned int UV2, unsigned int VERTEXCOLOR,
  unsigned int VERTEXALPHA, unsigned int MORPHTARGETS_TANGENT,
  unsigned int MORPHTARGETS_NORMAL, unsigned int MORPHTARGETS)
{
  if (!defines._areAttributesDirty && defines._needNormals == defines._normals
      && defines._needUVs == defines._uvs) {
    return false;
  }

  defines._normals = defines._needNormals;
  defines._uvs     = defines._needUVs;

  defines.defines[NORMAL]
    = (defines._needNormals
       && mesh->isVerticesDataPresent(VertexBuffer::NormalKind));

  if (defines._needNormals
      && mesh->isVerticesDataPresent(VertexBuffer::TangentKind)) {
    defines.TANGENT = true;
  }

  if (defines._needUVs) {
    defines.defines[UV1] = mesh->isVerticesDataPresent(VertexBuffer::UVKind);
    defines.defines[UV2] = mesh->isVerticesDataPresent(VertexBuffer::UV2Kind);
  }
  else {
    defines.defines[UV1] = false;
    defines.defines[UV2] = false;
  }

  if (useVertexColor) {
    auto hasVertexColors
      = mesh->useVertexColors()
        && mesh->isVerticesDataPresent(VertexBuffer::ColorKind);
    defines.defines[VERTEXCOLOR] = hasVertexColors;
    defines.defines[VERTEXALPHA] = mesh->hasVertexAlpha() && useVertexAlpha;
  }

  if (useBones) {
    if (mesh->useBones() && mesh->computeBonesUsingShaders()
        && mesh->skeleton()) {
      defines.NUM_BONE_INFLUENCERS = mesh->numBoneInfluencers();
      defines.BonesPerMesh
        = static_cast<unsigned>(mesh->skeleton()->bones.size() + 1);
    }
    else {
      defines.NUM_BONE_INFLUENCERS = 0;
      defines.BonesPerMesh         = 0;
    }
  }

  if (useMorphTargets) {
    auto _mesh   = static_cast<Mesh*>(mesh);
    auto manager = _mesh ? _mesh->morphTargetManager() : nullptr;
    if (manager) {
      defines.defines[MORPHTARGETS_TANGENT]
        = manager->supportsTangents() && defines.TANGENT;
      defines.defines[MORPHTARGETS_NORMAL]
        = manager->supportsNormals() && defines[NORMAL];
      defines.defines[MORPHTARGETS] = (manager->numInfluencers() > 0);
      defines.NUM_MORPH_INFLUENCERS
        = static_cast<unsigned>(manager->numInfluencers());
    }
    else {
      defines.defines[MORPHTARGETS_TANGENT] = false;
      defines.defines[MORPHTARGETS_NORMAL]  = false;
      defines.defines[MORPHTARGETS]         = false;
      defines.NUM_MORPH_INFLUENCERS         = 0;
    }
  }

  return true;
}

bool MaterialHelper::PrepareDefinesForLights(
  Scene* scene, AbstractMesh* mesh, MaterialDefines& defines,
  bool specularSupported, unsigned int maxSimultaneousLights,
  bool disableLighting, unsigned int SPECULARTERM, unsigned int SHADOWFLOAT)
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
  defines.resizeLights(maxSimultaneousLights - 1);

  if (scene->lightsEnabled() && !disableLighting) {
    for (auto& light : mesh->_lightSources) {
      needNormals = true;

      defines.lights[lightIndex] = true;

      defines.spotlights[lightIndex]  = false;
      defines.hemilights[lightIndex]  = false;
      defines.pointlights[lightIndex] = false;
      defines.dirlights[lightIndex]   = false;

      if (light->getTypeID() == Light::LIGHTTYPEID_SPOTLIGHT()) {
        defines.spotlights[lightIndex] = true;
        if (auto spotLight = static_cast<SpotLight*>(light)) {
          if (lightIndex >= defines.projectedLightTexture.size()) {
            defines.projectedLightTexture.resize(lightIndex + 1);
          }
          defines.projectedLightTexture[lightIndex]
            = spotLight->projectionTexture() ?
                spotLight->projectionTexture()->isReady() :
                false;
        }
      }
      else if (light->getTypeID() == Light::LIGHTTYPEID_HEMISPHERICLIGHT()) {
        defines.hemilights[lightIndex] = true;
      }
      else if (light->getTypeID() == Light::LIGHTTYPEID_POINTLIGHT()) {
        defines.pointlights[lightIndex] = true;
      }
      else {
        defines.dirlights[lightIndex] = true;
      }

      // Specular
      if (specularSupported && !light->specular.equalsFloats(0.f, 0.f, 0.f)) {
        specularEnabled = true;
      }

      // Shadows
      defines.shadows[lightIndex]               = false;
      defines.shadowpcfs[lightIndex]            = false;
      defines.shadowpcsss[lightIndex]           = false;
      defines.shadowpoissons[lightIndex]        = false;
      defines.shadowesms[lightIndex]            = false;
      defines.shadowcubes[lightIndex]           = false;
      defines.shadowlowqualities[lightIndex]    = false;
      defines.shadowmediumqualities[lightIndex] = false;

      if (mesh && mesh->receiveShadows() && scene->shadowsEnabled()
          && light->shadowEnabled) {
        auto shadowGenerator = light->getShadowGenerator();
        if (shadowGenerator) {
          shadowEnabled = true;
          shadowGenerator->prepareDefines(defines, lightIndex);
        }
      }

      if (light->lightmapMode() != Light::LIGHTMAP_DEFAULT()) {
        lightmapMode                         = true;
        defines.lightmapexcluded[lightIndex] = true;
        defines.lightmapnospecular[lightIndex]
          = (light->lightmapMode() == Light::LIGHTMAP_SHADOWSONLY());
      }
      else {
        defines.lightmapexcluded[lightIndex]   = false;
        defines.lightmapnospecular[lightIndex] = false;
      }

      ++lightIndex;
      if (lightIndex == maxSimultaneousLights) {
        break;
      }
    }
  }

  defines.defines[SPECULARTERM] = specularEnabled;
  defines.SHADOWS               = shadowEnabled;

  // Resetting all other lights if any
  for (unsigned int index = lightIndex; index < maxSimultaneousLights;
       ++index) {
    defines.lights[lightIndex]      = false;
    defines.hemilights[lightIndex]  = false;
    defines.pointlights[lightIndex] = false;
    defines.dirlights[lightIndex]   = false;
    defines.spotlights[lightIndex]  = false;
    defines.shadows[lightIndex]     = false;
  }

  auto caps = scene->getEngine()->getCaps();

  defines.defines[SHADOWFLOAT]
    = shadowEnabled
      && ((caps.textureFloatRender && caps.textureFloatLinearFiltering)
          || (caps.textureHalfFloatRender
              && caps.textureHalfFloatLinearFiltering));
  defines.LIGHTMAPEXCLUDED = lightmapMode;

  if (needRebuild) {
    defines.rebuild();
  }

  return needNormals;
}

void MaterialHelper::PrepareUniformsAndSamplersList(
  vector_t<string_t>& uniformsList, vector_t<string_t>& samplersList,
  MaterialDefines& defines, unsigned int maxSimultaneousLights)
{
  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    if (lightIndex >= defines.lights.size() || !defines.lights[lightIndex]) {
      break;
    }

    const string_t lightIndexStr = ::std::to_string(lightIndex);
    stl_util::concat(uniformsList,
                     {
                       "vLightData" + lightIndexStr,      //
                       "vLightDiffuse" + lightIndexStr,   //
                       "vLightSpecular" + lightIndexStr,  //
                       "vLightDirection" + lightIndexStr, //
                       "vLightGround" + lightIndexStr,    //
                       "lightMatrix" + lightIndexStr,     //
                       "shadowsInfo" + lightIndexStr,     //
                       "depthValues" + lightIndexStr      //
                     });

    samplersList.emplace_back("shadowSampler" + lightIndexStr);
    samplersList.emplace_back("depthSampler" + lightIndexStr);

    if (lightIndex < defines.projectedLightTexture.size()
        || !defines.projectedLightTexture[lightIndex]) {
      samplersList.emplace_back("projectionLightSampler" + lightIndexStr);
      uniformsList.emplace_back("textureProjectionMatrix" + lightIndexStr);
    }
  }

  if (defines.NUM_MORPH_INFLUENCERS > 0) {
    uniformsList.emplace_back("morphTargetInfluences");
  }
}

void MaterialHelper::PrepareUniformsAndSamplersList(
  EffectCreationOptions& options)
{
  auto& uniformsList          = options.uniformsNames;
  auto& uniformBuffersList    = options.uniformBuffersNames;
  auto& samplersList          = options.samplers;
  auto& defines               = options.materialDefines;
  auto& maxSimultaneousLights = options.maxSimultaneousLights;

  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    if (defines
        && (lightIndex >= defines->lights.size()
            || !defines->lights[lightIndex])) {
      break;
    }

    const string_t lightIndexStr = ::std::to_string(lightIndex);
    stl_util::concat(uniformsList,
                     {
                       "vLightData" + lightIndexStr,      //
                       "vLightDiffuse" + lightIndexStr,   //
                       "vLightSpecular" + lightIndexStr,  //
                       "vLightDirection" + lightIndexStr, //
                       "vLightGround" + lightIndexStr,    //
                       "lightMatrix" + lightIndexStr,     //
                       "shadowsInfo" + lightIndexStr,     //
                       "depthValues" + lightIndexStr      //
                     });

    uniformBuffersList.emplace_back("Light" + lightIndexStr);

    samplersList.emplace_back("shadowSampler" + lightIndexStr);
    samplersList.emplace_back("depthSampler" + lightIndexStr);

    if (lightIndex < defines->projectedLightTexture.size()
        && defines->projectedLightTexture[lightIndex]) {
      samplersList.emplace_back("projectionLightSampler" + lightIndexStr);
      uniformsList.emplace_back("textureProjectionMatrix" + lightIndexStr);
    }
  }

  if (defines->NUM_MORPH_INFLUENCERS > 0) {
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
    if (lightIndex >= defines.lights.size() || !defines.lights[lightIndex]) {
      break;
    }

    const string_t lightIndexStr = ::std::to_string(lightIndex);

    if (lightIndex > 0) {
      lightFallbackRank = rank + lightIndex;
      fallbacks.addFallback(lightFallbackRank, "LIGHT" + lightIndexStr);
    }

    if (!defines.SHADOWS) {
      if (defines.shadows[lightIndex]) {
        fallbacks.addFallback(rank, "SHADOW" + lightIndexStr);
      }

      if (defines.shadowpcfs[lightIndex]) {
        fallbacks.addFallback(rank, "SHADOWPCF" + lightIndexStr);
      }

      if (defines.shadowpcsss[lightIndex]) {
        fallbacks.addFallback(rank, "SHADOWPCSS" + lightIndexStr);
      }

      if (defines.shadowpoissons[lightIndex]) {
        fallbacks.addFallback(rank, "SHADOWPOISSON" + lightIndexStr);
      }

      if (defines.shadowesms[lightIndex]) {
        fallbacks.addFallback(rank, "SHADOWESM" + lightIndexStr);
      }
    }
  }
  return lightFallbackRank++;
}

void MaterialHelper::PrepareAttributesForMorphTargets(
  vector_t<string_t>& attribs, AbstractMesh* mesh, MaterialDefines& defines,
  unsigned int NORMAL)
{
  auto influencers = defines.NUM_MORPH_INFLUENCERS;

  auto engine = Engine::LastCreatedEngine();
  auto _mesh  = static_cast<Mesh*>(mesh);
  if (influencers > 0 && engine && mesh) {
    auto maxAttributesCount
      = static_cast<unsigned>(engine->getCaps().maxVertexAttribs);
    auto manager = _mesh->morphTargetManager();
    auto normal  = manager && manager->supportsNormals() && defines[NORMAL];
    auto tangent = manager && manager->supportsNormals() && defines.TANGENT;
    for (unsigned int index = 0; index < influencers; index++) {
      const auto indexStr = ::std::to_string(index);
      attribs.emplace_back(string_t(VertexBuffer::PositionKindChars)
                           + indexStr);

      if (normal) {
        attribs.emplace_back(string_t(VertexBuffer::NormalKindChars)
                             + indexStr);
      }

      if (tangent) {
        attribs.emplace_back(string_t(VertexBuffer::TangentKindChars)
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

void MaterialHelper::PrepareAttributesForBones(vector_t<string_t>& attribs,
                                               AbstractMesh* mesh,
                                               MaterialDefines& defines,
                                               EffectFallbacks& fallbacks)
{
  if (defines.NUM_BONE_INFLUENCERS > 0) {
    fallbacks.addCPUSkinningFallback(0, mesh);

    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (defines.NUM_BONE_INFLUENCERS > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
  }
}

void MaterialHelper::PrepareAttributesForInstances(vector_t<string_t>& attribs,
                                                   MaterialDefines& defines,
                                                   unsigned int INSTANCES)
{
  if (defines[INSTANCES]) {
    attribs.emplace_back(VertexBuffer::World0KindChars);
    attribs.emplace_back(VertexBuffer::World1KindChars);
    attribs.emplace_back(VertexBuffer::World2KindChars);
    attribs.emplace_back(VertexBuffer::World3KindChars);
  }
}

void MaterialHelper::BindLightShadow(Light* light, Scene* /*scene*/,
                                     AbstractMesh* mesh,
                                     unsigned int lightIndex, Effect* effect)
{
  if (light->shadowEnabled && mesh->receiveShadows()) {
    auto shadowGenerator = light->getShadowGenerator();
    if (shadowGenerator) {
      shadowGenerator->bindShadowLight(::std::to_string(lightIndex), effect);
    }
  }
}

void MaterialHelper::BindLightProperties(Light* light, Effect* effect,
                                         unsigned int lightIndex)
{
  light->transferToEffect(effect, ::std::to_string(lightIndex));
}

void MaterialHelper::BindLights(Scene* scene, AbstractMesh* mesh,
                                Effect* effect, MaterialDefines& defines,
                                unsigned int maxSimultaneousLights,
                                unsigned int SPECULARTERM,
                                bool usePhysicalLightFalloff)
{
  auto len = ::std::min(mesh->_lightSources.size(),
                        static_cast<size_t>(maxSimultaneousLights));

  for (unsigned int i = 0; i < len; i++) {

    auto& light    = mesh->_lightSources[i];
    auto iAsString = ::std::to_string(i);

    auto scaledIntensity = light->getScaledIntensity();
    light->_uniformBuffer->bindToEffect(effect, "Light" + iAsString);

    MaterialHelper::BindLightProperties(light, effect, i);

    light->diffuse.scaleToRef(scaledIntensity, Tmp::Color3Array[0]);
    light->_uniformBuffer->updateColor4(
      "vLightDiffuse", Tmp::Color3Array[0],
      usePhysicalLightFalloff ? light->radius() : light->range, iAsString);
    if (defines[SPECULARTERM]) {
      light->specular.scaleToRef(scaledIntensity, Tmp::Color3Array[1]);
      light->_uniformBuffer->updateColor3("vLightSpecular", Tmp::Color3Array[1],
                                          iAsString);
    }

    // Shadows
    if (scene->shadowsEnabled()) {
      BindLightShadow(light, scene, mesh, i, effect);
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
  if (mesh && mesh->useBones() && mesh->computeBonesUsingShaders()
      && mesh->skeleton()) {
    const auto& matrices = mesh->skeleton()->getTransformMatrices(mesh);
    if (!matrices.empty() && effect) {
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
                                  Scene* scene, unsigned int LOGARITHMICDEPTH)
{
  if (defines[LOGARITHMICDEPTH]) {
    effect->setFloat(
      "logarithmicDepthConstant",
      2.f / (::std::log(scene->activeCamera->maxZ + 1.f) / Math::LN2));
  }
}

void MaterialHelper::BindClipPlane(Effect* effect, Scene* scene)
{
  if (scene->clipPlane()) {
    auto clipPlane = scene->clipPlane();
    effect->setFloat4("vClipPlane", clipPlane->normal.x, clipPlane->normal.y,
                      clipPlane->normal.z, clipPlane->d);
  }
}

} // end of namespace BABYLON
