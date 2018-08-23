#include <babylon/layer/layer_scene_component.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/layer/layer.h>

namespace BABYLON {

LayerSceneComponent::LayerSceneComponent(Scene* iScene)
{
  scene   = iScene;
  _engine = scene->getEngine();
}

LayerSceneComponent::~LayerSceneComponent()
{
}

void LayerSceneComponent::_register()
{
  scene->_beforeCameraDrawStage.registerStep(
    SceneComponentConstants::STEP_BEFORECAMERADRAW_LAYER, this,
    [this](Camera* camera) { _drawBackground(camera); });
  scene->_afterCameraDrawStage.registerStep(
    SceneComponentConstants::STEP_AFTERCAMERADRAW_LAYER, this,
    [this](Camera* camera) { _drawForeground(camera); });
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

void LayerSceneComponent::_draw(Camera* camera, bool isBackground)
{
  const auto& layers = scene->layers;

  if (!layers.empty()) {
    _engine->setDepthBuffer(false);
    auto cameraLayerMask = camera->layerMask;
    for (const auto& layer : layers) {
      if (layer->isBackground == isBackground
          && ((layer->layerMask & cameraLayerMask) != 0)) {
        layer->render();
      }
    }
    _engine->setDepthBuffer(true);
  }
}

void LayerSceneComponent::_drawBackground(Camera* camera)
{
  _draw(camera, true);
}

void LayerSceneComponent::_drawForeground(Camera* camera)
{
  _draw(camera, false);
}

} // end of namespace BABYLON
