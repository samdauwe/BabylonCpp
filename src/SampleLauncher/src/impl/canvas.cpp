#include <babylon/impl/canvas.h>

#include <babylon/impl/gl_rendering_context.h>

namespace BABYLON {
namespace impl {

Canvas::Canvas() : ICanvas{}
{
  _renderingContext          = std::make_unique<GL::GLRenderingContext>();
  _boundingClientRect.bottom = clientHeight;
  _boundingClientRect.height = clientHeight;
  _boundingClientRect.left   = 0;
  _boundingClientRect.right  = clientWidth;
  _boundingClientRect.top    = 0;
  _boundingClientRect.width  = clientWidth;
}

Canvas::~Canvas()
{
}

bool Canvas::initializeContext3d()
{
  if (!_initialized) {
    _initialized = _renderingContext->initialize();
  }

  return _initialized;
}

ClientRect& Canvas::getBoundingClientRect()
{
  return _boundingClientRect;
}

bool Canvas::onlyRenderBoundingClientRect() const
{
  return false;
}

ICanvasRenderingContext2D* Canvas::getContext2d()
{
  return nullptr;
}

GL::IGLRenderingContext* Canvas::getContext3d(const EngineOptions& /*options*/)
{
  return _renderingContext.get();
}

} // end of namespace impl
} // end of namespace BABYLON
