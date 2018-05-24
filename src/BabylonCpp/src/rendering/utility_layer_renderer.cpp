#include <babylon/rendering/utility_layer_renderer.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

UtilityLayerRenderer::UtilityLayerRenderer(Scene* iOriginalScene)
    : shouldRender{true}, originalScene{iOriginalScene}
{
  // Create scene which will be rendered in the foreground and remove it from
  // being referenced by engine to avoid interfering with existing app
  utilityLayerScene = Scene::New(iOriginalScene->getEngine());
  iOriginalScene->getEngine()->scenes.pop_back();

  // Render directly on top of existing scene without clearing
  utilityLayerScene->autoClear = false;

  _afterRenderObserver = originalScene->onAfterRenderObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) {
      if (shouldRender) {
        render();
      }
    });

  _sceneDisposeObserver = originalScene->onDisposeObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) { dispose(); });
}

UtilityLayerRenderer::~UtilityLayerRenderer()
{
}

void UtilityLayerRenderer::render()
{
  _updateCamera();
  utilityLayerScene->render(false);
}

void UtilityLayerRenderer::dispose(bool /*doNotRecurse*/,
                                   bool /*disposeMaterialAndTextures*/)
{
  if (_afterRenderObserver) {
    originalScene->onAfterRenderObservable.remove(_afterRenderObserver);
  }
  if (_sceneDisposeObserver) {
    originalScene->onDisposeObservable.remove(_sceneDisposeObserver);
  }
  utilityLayerScene->dispose();
}

void UtilityLayerRenderer::_updateCamera()
{
  utilityLayerScene->activeCamera = originalScene->activeCamera;
}

} // end of namespace BABYLON
