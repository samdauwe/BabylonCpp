#include <babylon/rendering/depth_renderer_scene_component.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

DepthRendererSceneComponent::DepthRendererSceneComponent(Scene* iScene)
{
  ISceneComponent::name = DepthRendererSceneComponent::name;
  scene                 = iScene;
}

DepthRendererSceneComponent::~DepthRendererSceneComponent() = default;

void DepthRendererSceneComponent::_register()
{
  scene->_gatherRenderTargetsStage.registerStep(
    SceneComponentConstants::STEP_GATHERRENDERTARGETS_DEPTHRENDERER, this,
    [this](std::vector<RenderTargetTexturePtr>& renderTargets) {
      _gatherRenderTargets(renderTargets);
    });
  scene->_gatherActiveCameraRenderTargetsStage.registerStep(
    SceneComponentConstants::STEP_GATHERACTIVECAMERARENDERTARGETS_DEPTHRENDERER,
    this, [this](std::vector<RenderTargetTexturePtr>& renderTargets) {
      _gatherActiveCameraRenderTargets(renderTargets);
    });
}

void DepthRendererSceneComponent::rebuild()
{
  // Nothing to do for this component
}

void DepthRendererSceneComponent::dispose()
{
  for (const auto& item : scene->depthRenderer()) {
    item.second->dispose();
  }
}

void DepthRendererSceneComponent::_gatherRenderTargets(
  std::vector<RenderTargetTexturePtr>& renderTargets)
{
  if (!scene->depthRenderer().empty()) {
    for (const auto& item : scene->depthRenderer()) {
      const auto& depthRenderer = item.second;
      if (!depthRenderer->useOnlyInActiveCamera) {
        renderTargets.emplace_back(depthRenderer->getDepthMap());
      }
    }
  }
}

void DepthRendererSceneComponent::_gatherActiveCameraRenderTargets(
  std::vector<RenderTargetTexturePtr>& renderTargets)
{
  if (!scene->depthRenderer().empty()) {
    for (const auto& item : scene->depthRenderer()) {
      const auto& depthRenderer = item.second;
      if (depthRenderer->useOnlyInActiveCamera
          && scene->activeCamera()->id == item.first) {
        renderTargets.emplace_back(depthRenderer->getDepthMap());
      }
    }
  }
}

} // end of namespace BABYLON
