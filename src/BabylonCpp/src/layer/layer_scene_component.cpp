#include <babylon/layer/layer_scene_component.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/layer/layer.h>

namespace BABYLON {

LayerSceneComponent::LayerSceneComponent(Scene* iScene)
{
  ISceneComponent::name = LayerSceneComponent::name;
  scene                 = iScene;
  _engine               = scene->getEngine();
}

LayerSceneComponent::~LayerSceneComponent()
{
}

void LayerSceneComponent::_register()
{
  scene->_beforeCameraDrawStage.registerStep(
    SceneComponentConstants::STEP_BEFORECAMERADRAW_LAYER, this,
    [this](Camera* camera) -> void { _drawCameraBackground(camera); });
  scene->_afterCameraDrawStage.registerStep(
    SceneComponentConstants::STEP_AFTERCAMERADRAW_LAYER, this,
    [this](Camera* camera) -> void { _drawCameraForeground(camera); });

  scene->_beforeRenderTargetDrawStage.registerStep(
    SceneComponentConstants::STEP_BEFORERENDERTARGETDRAW_LAYER, this,
    [this](const RenderTargetTexturePtr& renderTarget) -> void {
      _drawRenderTargetBackground(renderTarget);
    });
  scene->_afterRenderTargetDrawStage.registerStep(
    SceneComponentConstants::STEP_AFTERRENDERTARGETDRAW_LAYER, this,
    [this](const RenderTargetTexturePtr& renderTarget) -> void {
      _drawRenderTargetForeground(renderTarget);
    });
}

void LayerSceneComponent::rebuild()
{
  const auto& layers = scene->layers;

  for (const auto& layer : layers) {
    layer->_rebuild();
  }
}

void LayerSceneComponent::dispose()
{
  auto& layers = scene->layers;

  for (const auto& layer : layers) {
    layer->dispose();
  }

  layers.clear();
}

void LayerSceneComponent::_draw(
  const std::function<bool(const Layer& layer)>& predicate)
{
  const auto& layers = scene->layers;

  if (!layers.empty()) {
    _engine->setDepthBuffer(false);
    for (const auto& layer : layers) {
      if (predicate(*layer)) {
        layer->render();
      }
    }
    _engine->setDepthBuffer(true);
  }
}

bool LayerSceneComponent::_drawCameraPredicate(
  const Layer& layer, bool isBackground, unsigned int cameraLayerMask) const
{
  return !layer.renderOnlyInRenderTargetTextures
         && layer.isBackground == isBackground
         && ((layer.layerMask & cameraLayerMask) != 0);
}

void LayerSceneComponent::_drawCameraBackground(Camera* camera)
{
  _draw([this, &camera](const Layer& layer) -> bool {
    return _drawCameraPredicate(layer, true, camera->layerMask);
  });
}

void LayerSceneComponent::_drawCameraForeground(Camera* camera)
{
  _draw([this, &camera](const Layer& layer) -> bool {
    return _drawCameraPredicate(layer, false, camera->layerMask);
  });
}

bool LayerSceneComponent::_drawRenderTargetPredicate(
  const Layer& layer, bool isBackground, unsigned int cameraLayerMask,
  const RenderTargetTexturePtr& renderTargetTexture) const
{
  return (layer.renderTargetTextures.size() > 0)
         && layer.isBackground == isBackground
         && (stl_util::index_of(layer.renderTargetTextures, renderTargetTexture)
             > -1)
         && ((layer.layerMask & cameraLayerMask) != 0);
}

void LayerSceneComponent::_drawRenderTargetBackground(
  const RenderTargetTexturePtr& renderTarget)
{
  _draw([this, &renderTarget](const Layer& layer) -> bool {
    return _drawRenderTargetPredicate(
      layer, true, scene->activeCamera->layerMask, renderTarget);
  });
}

void LayerSceneComponent::_drawRenderTargetForeground(
  const RenderTargetTexturePtr& renderTarget)
{
  _draw([this, &renderTarget](const Layer& layer) -> bool {
    return _drawRenderTargetPredicate(
      layer, false, scene->activeCamera->layerMask, renderTarget);
  });
}

} // end of namespace BABYLON
