#include <babylon/materials/material_helper.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/ishadow_light.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/plane.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

bool MaterialHelper::PrepareDefinesForLights(Scene* scene, AbstractMesh* mesh,
                                             MaterialDefines& defines,
                                             unsigned int maxSimultaneousLights,
                                             int SPECULARTERM, int SHADOWS,
                                             int SHADOWFULLFLOAT)
{
  unsigned int lightIndex = 0;
  bool needNormals        = false;
  bool needShadows        = false;
  bool lightmapMode       = false;
  defines.resizeLights(maxSimultaneousLights - 1);

  for (auto& light : scene->lights) {

    if (!light->isEnabled()) {
      continue;
    }

    // Excluded check
    if (!light->_excludedMeshesIds.empty()) {
      for (unsigned int excludedIndex = 0;
           excludedIndex < light->_excludedMeshesIds.size(); ++excludedIndex) {
        auto excludedMesh
          = scene->getMeshByID(light->_excludedMeshesIds[excludedIndex]);

        if (excludedMesh) {
          light->excludedMeshes.emplace_back(excludedMesh);
        }
      }

      light->_excludedMeshesIds.clear();
    }

    // Included check
    if (!light->_includedOnlyMeshesIds.empty()) {
      for (unsigned int includedOnlyIndex = 0;
           includedOnlyIndex < light->_includedOnlyMeshesIds.size();
           ++includedOnlyIndex) {
        auto includedOnlyMesh = scene->getMeshByID(
          light->_includedOnlyMeshesIds[includedOnlyIndex]);

        if (includedOnlyMesh) {
          light->includedOnlyMeshes.emplace_back(includedOnlyMesh);
        }
      }

      light->_includedOnlyMeshesIds.clear();
    }

    if (!light->canAffectMesh(mesh)) {
      continue;
    }
    needNormals = true;

    defines.lights[lightIndex] = true;

    switch (light->type()) {
      case Node::Type::SPOTLIGHT:
        defines.spotlights[lightIndex] = true;
        break;
      case Node::Type::HEMISPHERICLIGHT:
        defines.hemilights[lightIndex] = true;
        break;
      case Node::Type::POINTLIGHT:
        defines.pointlights[lightIndex] = true;
        break;
      case Node::Type::DIRECTIONALLIGHT:
      default:
        defines.dirlights[lightIndex] = true;
    }

    // Specular
    if (!light->specular.equalsFloats(0.f, 0.f, 0.f) && SPECULARTERM > -1) {
      defines.defines[static_cast<unsigned int>(SPECULARTERM)] = true;
    }

    // Shadows
    if (scene->shadowsEnabled) {
      auto shadowGenerator = light->getShadowGenerator();
      if (mesh && mesh->receiveShadows && shadowGenerator) {
        defines.shadows[lightIndex] = true;

        defines.defines[static_cast<unsigned int>(SHADOWS)] = true;

        if (shadowGenerator->useVarianceShadowMap()
            || shadowGenerator->useBlurVarianceShadowMap()) {
          defines.shadowvsms[lightIndex] = true;
        }

        if (shadowGenerator->usePoissonSampling()) {
          defines.shadowpcfs[lightIndex] = true;
        }

        needShadows = true;
      }
    }

    if (light->lightmapMode != Light::LIGHTMAP_DEFAULT) {
      lightmapMode                         = true;
      defines.lightmapexcluded[lightIndex] = true;
      if (light->lightmapMode == Light::LIGHTMAP_SHADOWSONLY) {
        defines.lightmapnospecular[lightIndex] = true;
      }
    }

    ++lightIndex;
    if (lightIndex == maxSimultaneousLights) {
      break;
    }
  }

  auto caps = scene->getEngine()->getCaps();
  if (needShadows && caps.textureFloat && caps.textureFloatLinearFiltering
      && caps.textureFloatRender && SHADOWFULLFLOAT != -1) {
    defines.defines[static_cast<unsigned int>(SHADOWFULLFLOAT)] = true;
  }

  if (lightmapMode) {
    defines.LIGHTMAPEXCLUDED = true;
  }

  return needNormals;
}

void MaterialHelper::PrepareUniformsAndSamplersList(
  std::vector<std::string>& uniformsList,
  std::vector<std::string>& samplersList, MaterialDefines& defines,
  unsigned int maxSimultaneousLights)
{
  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    if ((lightIndex >= defines.lights.size())
        || (!defines.lights[lightIndex])) {
      break;
    }

    const std::string lightIndexStr = std::to_string(lightIndex);

    std_util::concat(uniformsList, {"vLightData" + lightIndexStr,      //
                                    "vLightDiffuse" + lightIndexStr,   //
                                    "vLightSpecular" + lightIndexStr,  //
                                    "vLightDirection" + lightIndexStr, //
                                    "vLightGround" + lightIndexStr,    //
                                    "lightMatrix" + lightIndexStr,     //
                                    "shadowsInfo" + lightIndexStr});

    samplersList.emplace_back("shadowSampler" + lightIndexStr);
  }
}

void MaterialHelper::HandleFallbacksForShadows(
  MaterialDefines& defines, EffectFallbacks* fallbacks,
  unsigned int maxSimultaneousLights)
{
  for (unsigned int lightIndex = 0; lightIndex < maxSimultaneousLights;
       ++lightIndex) {
    if ((lightIndex >= defines.lights.size())
        || (!defines.lights[lightIndex])) {
      break;
    }

    const std::string lightIndexStr = std::to_string(lightIndex);

    if (lightIndex > 0) {
      fallbacks->addFallback(lightIndex, "LIGHT" + lightIndexStr);
    }

    if (defines.shadows[lightIndex]) {
      fallbacks->addFallback(0, "SHADOW" + lightIndexStr);
    }

    if (defines.shadowpcfs[lightIndex]) {
      fallbacks->addFallback(0, "SHADOWPCF" + lightIndexStr);
    }

    if (defines.shadowvsms[lightIndex]) {
      fallbacks->addFallback(0, "SHADOWVSM" + lightIndexStr);
    }
  }
}

void MaterialHelper::PrepareAttributesForBones(
  std::vector<std::string>& attribs, AbstractMesh* mesh,
  std::size_t numBoneInfluencers, EffectFallbacks* fallbacks)
{
  if (numBoneInfluencers > 0) {
    fallbacks->addCPUSkinningFallback(0, mesh);

    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (numBoneInfluencers > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
  }
}

void MaterialHelper::PrepareAttributesForInstances(
  std::vector<std::string>& attribs, MaterialDefines& defines, int INSTANCES)
{
  if ((INSTANCES > 0) && defines[static_cast<unsigned int>(INSTANCES)]) {
    attribs.emplace_back(VertexBuffer::World0KindChars);
    attribs.emplace_back(VertexBuffer::World1KindChars);
    attribs.emplace_back(VertexBuffer::World2KindChars);
    attribs.emplace_back(VertexBuffer::World3KindChars);
  }
}

bool MaterialHelper::BindLightShadow(Light* light, Scene* scene,
                                     AbstractMesh* mesh,
                                     unsigned int lightIndex, Effect* effect,
                                     bool _depthValuesAlreadySet)
{
  bool depthValuesAlreadySet       = _depthValuesAlreadySet;
  ShadowGenerator* shadowGenerator = light->getShadowGenerator();
  const std::string lightIndexStr  = std::to_string(lightIndex);

  if (mesh->receiveShadows && shadowGenerator) {
    IShadowLight* shadowLight = dynamic_cast<IShadowLight*>(light);
    if (shadowLight && !shadowLight->needCube()) {
      effect->setMatrix("lightMatrix" + lightIndexStr,
                        shadowGenerator->getTransformMatrix());
    }
    else {
      if (!depthValuesAlreadySet) {
        depthValuesAlreadySet = true;
        effect->setFloat2("depthValues", scene->activeCamera->minZ,
                          scene->activeCamera->maxZ);
      }
    }
    effect->setTexture("shadowSampler" + lightIndexStr,
                       shadowGenerator->getShadowMapForRendering());
    effect->setFloat3(
      "shadowsInfo" + lightIndexStr, shadowGenerator->getDarkness(),
      shadowGenerator->blurScale
        / static_cast<float>(shadowGenerator->getShadowMap()->getSize().width),
      shadowGenerator->bias());
  }

  return depthValuesAlreadySet;
}

void MaterialHelper::BindLightProperties(Light* light, Effect* effect,
                                         unsigned int lightIndex)
{
  const std::string lightIndexStr = std::to_string(lightIndex);

  switch (light->type()) {
    case Node::Type::POINTLIGHT:
      // Point Light
      light->transferToEffect(effect, "vLightData" + lightIndexStr);
      break;
    case Node::Type::DIRECTIONALLIGHT:
      // Directional Light
      light->transferToEffect(effect, "vLightData" + lightIndexStr);
      break;
    case Node::Type::SPOTLIGHT:
      // Spot Light
      light->transferToEffect(effect, "vLightData" + lightIndexStr,
                              "vLightDirection" + lightIndexStr);
      break;
    case Node::Type::HEMISPHERICLIGHT:
      // Hemispheric Light
      light->transferToEffect(effect, "vLightData" + lightIndexStr,
                              "vLightGround" + lightIndexStr);
      break;
    default:
      break;
  }
}

void MaterialHelper::BindLights(Scene* scene, AbstractMesh* mesh,
                                Effect* effect, bool specularTerm,
                                unsigned int maxSimultaneousLights)
{
  unsigned int lightIndex    = 0;
  bool depthValuesAlreadySet = false;
  for (auto& light : scene->lights) {
    if (!light->isEnabled()) {
      continue;
    }

    if (!light->canAffectMesh(mesh)) {
      continue;
    }

    BindLightProperties(light.get(), effect, lightIndex);

    const std::string lightIndexStr = std::to_string(lightIndex);

    light->diffuse.scaleToRef(light->intensity, Tmp::Color3Array[0]);
    effect->setColor4("vLightDiffuse" + lightIndexStr, Tmp::Color3Array[0],
                      light->range);
    if (specularTerm) {
      light->specular.scaleToRef(light->intensity, Tmp::Color3Array[1]);
      effect->setColor3("vLightSpecular" + lightIndexStr, Tmp::Color3Array[1]);
    }

    // Shadows
    if (scene->shadowsEnabled) {
      depthValuesAlreadySet = BindLightShadow(
        light.get(), scene, mesh, lightIndex, effect, depthValuesAlreadySet);
    }

    ++lightIndex;

    if (lightIndex == maxSimultaneousLights) {
      break;
    }
  }
}

void MaterialHelper::BindFogParameters(Scene* scene, AbstractMesh* mesh,
                                       Effect* effect)
{
  if (scene->fogEnabled && mesh->applyFog
      && scene->fogMode != Scene::FOGMODE_NONE) {
    effect->setFloat4("vFogInfos", static_cast<float>(scene->fogMode),
                      scene->fogStart, scene->fogEnd, scene->fogDensity);
    effect->setColor3("vFogColor", scene->fogColor);
  }
}

void MaterialHelper::BindBonesParameters(AbstractMesh* mesh, Effect* effect)
{
  if (mesh && mesh->useBones() && mesh->computeBonesUsingShaders) {
    const auto& matrices = mesh->skeleton()->getTransformMatrices(mesh);
    if (!matrices.empty()) {
      effect->setMatrices("mBones", matrices);
    }
  }
}

void MaterialHelper::BindLogDepth(bool logarithmicDepth, Effect* effect,
                                  Scene* scene)
{
  if (logarithmicDepth) {
    effect->setFloat(
      "logarithmicDepthConstant",
      2.f / (std::log(scene->activeCamera->maxZ + 1.f) / Math::LN2));
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
