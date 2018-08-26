#include <babylon/rendering/depth_renderer_scene_component.h>

#include <babylon/engine/scene.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

DepthRendererSceneComponent::DepthRendererSceneComponent(Scene* iScene)
{
  scene = iScene;
}

DepthRendererSceneComponent::~DepthRendererSceneComponent()
{
}

void DepthRendererSceneComponent::_register()
{
  scene->_gatherRenderTargetsStage.registerStep(
    SceneComponentConstants::STEP_GATHERRENDERTARGETS_DEPTHRENDERER, this,
    [this](vector_t<RenderTargetTexturePtr>& renderTargets) {
      _gatherRenderTargets(renderTargets);
    });
}

void DepthRendererSceneComponent::rebuild()
{
  // Nothing to do for this component
}

void DepthRendererSceneComponent::dispose()
{
  for (auto& item : scene->depthRenderer()) {
    item.second->dispose();
  }
}

void DepthRendererSceneComponent::_gatherRenderTargets(
  vector_t<RenderTargetTexturePtr>& renderTargets)
{
  if (!scene->depthRenderer().empty()) {
    for (auto& item : scene->depthRenderer()) {
      renderTargets.emplace_back(item.second->getDepthMap());
    }
  }
}

} // end of namespace BABYLON
