#include <babylon/misc/depth_reducer.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/rendering/depth_renderer.h>

namespace BABYLON {

DepthReducer::DepthReducer(const CameraPtr& camera)
    : MinMaxReducer{camera}
    , depthRenderer{this, &DepthReducer::get_depthRenderer}
    , _depthRenderer{nullptr}
{
}

DepthReducer::~DepthReducer() = default;

DepthRendererPtr& DepthReducer::get_depthRenderer()
{
  return _depthRenderer;
}

void DepthReducer::setDepthRenderer(DepthRendererPtr depthRenderer, unsigned int type,
                                    bool forceFullscreenViewport)
{
  auto scene = _camera->getScene();

  if (_depthRenderer) {
    scene->_depthRenderer.erase(_depthRendererId);

    _depthRenderer->dispose();
    _depthRenderer = nullptr;
  }

  if (depthRenderer == nullptr) {
    if (scene->_depthRenderer.empty()) {
      scene->_depthRenderer = {};
    }

    depthRenderer = _depthRenderer = std::make_shared<DepthRenderer>(scene, type, _camera, false);
    depthRenderer->enabled         = false;

    _depthRendererId                        = "minmax" + _camera->id;
    scene->_depthRenderer[_depthRendererId] = depthRenderer;
  }

  MinMaxReducer::setSourceTexture(depthRenderer->getDepthMap(), true, type,
                                  forceFullscreenViewport);
}

void DepthReducer::setSourceTexture(const RenderTargetTexturePtr& sourceTexture, bool depthRedux,
                                    unsigned int type, bool forceFullscreenViewport)
{
  MinMaxReducer::setSourceTexture(sourceTexture, depthRedux, type, forceFullscreenViewport);
}

void DepthReducer::activate()
{
  if (_depthRenderer) {
    _depthRenderer->enabled = true;
  }

  MinMaxReducer::activate();
}

void DepthReducer::deactivate()
{
  MinMaxReducer::deactivate();

  if (_depthRenderer) {
    _depthRenderer->enabled = false;
  }
}

void DepthReducer::dispose(bool disposeAll)
{
  MinMaxReducer::dispose(disposeAll);

  if (_depthRenderer && disposeAll) {
    _depthRenderer->getDepthMap()->getScene()->_depthRenderer.erase(_depthRendererId);

    _depthRenderer->dispose();
    _depthRenderer = nullptr;
  }
}

} // end of namespace BABYLON
