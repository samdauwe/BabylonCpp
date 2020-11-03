#include "basic_canvas.h"
#include "gl_rendering_context.h"

using namespace BABYLON;

BasicCanvas::BasicCanvas() : ICanvas{}
{
  _renderingContext          = std::make_unique<GL::GLRenderingContext>();
  _boundingClientRect.bottom = clientHeight;
  _boundingClientRect.height = clientHeight;
  _boundingClientRect.left   = 0;
  _boundingClientRect.right  = clientWidth;
  _boundingClientRect.top    = 0;
  _boundingClientRect.width  = clientWidth;
}

BasicCanvas::~BasicCanvas()
{
}

bool BasicCanvas::initializeContext3d()
{
  if (!_initialized) {
    _initialized = _renderingContext->initialize();
  }
  return _initialized;
}

ClientRect& BasicCanvas::getBoundingClientRect()
{
  return _boundingClientRect;
}

ICanvasRenderingContext2D* BasicCanvas::getContext2d()
{
  return nullptr;
}

GL::IGLRenderingContext* BasicCanvas::getContext3d(const EngineOptions& /*options*/)
{
  return _renderingContext.get();
}
