#include <babylon/lights/shadows/shadow_generator_scene_component.h>

#include <babylon/engine/scene.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/textures/render_target_texture.h>

namespace BABYLON {

ShadowGeneratorSceneComponent::ShadowGeneratorSceneComponent(Scene* iScene)
{
  scene = iScene;
}

ShadowGeneratorSceneComponent::~ShadowGeneratorSceneComponent()
{
}

void ShadowGeneratorSceneComponent::_register()
{
  scene->_gatherRenderTargetsStage.registerStep(
    SceneComponentConstants::STEP_GATHERRENDERTARGETS_SHADOWGENERATOR, this,
    [this](std::vector<RenderTargetTexturePtr>& renderTargets) {
      _gatherRenderTargets(renderTargets);
    });
}

void ShadowGeneratorSceneComponent::rebuild()
{
  // Nothing To Do Here.
}

void ShadowGeneratorSceneComponent::serialize(json& /*serializationObject*/)
{
}

void ShadowGeneratorSceneComponent::addFromContainer(
  AbstractScene& /*container*/)
{
  // Nothing To Do Here. (directly attached to a light)
}

void ShadowGeneratorSceneComponent::removeFromContainer(
  AbstractScene& /*container*/)
{
  // Nothing To Do Here. (directly attached to a light)
}

void ShadowGeneratorSceneComponent::dispose()
{
  // Nothing To Do Here.
}

void ShadowGeneratorSceneComponent::_gatherRenderTargets(
  std::vector<RenderTargetTexturePtr>& renderTargets)
{
  // Shadows
  if (scene->shadowsEnabled) {
    for (const auto& light : scene->lights) {
      auto shadowGenerator = light->getShadowGenerator();

      if (light->isEnabled() && light->shadowEnabled && shadowGenerator) {
        auto shadowMap = shadowGenerator->getShadowMap();
        auto it = std::find_if(scene->textures.begin(), scene->textures.end(),
                               [&shadowMap](const BaseTexturePtr& texture) {
                                 return texture == shadowMap;
                               });
        if (it != scene->textures.end()) {
          renderTargets.emplace_back(shadowMap);
        }
      }
    }
  }
}

} // end of namespace BABYLON
