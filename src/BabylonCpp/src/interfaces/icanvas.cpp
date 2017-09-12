#include <babylon/interfaces/icanvas.h>

#include <babylon/core/logging.h>

namespace BABYLON {

ICanvas::ICanvas()
    : width{0}
    , height{0}
    , clientWidth{0}
    , clientHeight{0}
    , mouseX{0}
    , mouseY{0}
    , _initialized{false}
{
  mouseEventListeners.resize(static_cast<unsigned>(EventType::MOUSE_WHEEL) + 1);
  keyEventListeners.resize(static_cast<unsigned>(EventType::KEY_UP)
                           - static_cast<unsigned>(EventType::MOUSE_WHEEL));
}

ICanvas::~ICanvas()
{
}

void ICanvas::addMouseEventListener(
  EventType /*type*/, const FastFunc<void(PointerEvent&& evt)>& /*listener*/,
  bool /*useCapture*/)
{
#if 0
  auto& listeners = mouseEventListeners[static_cast<unsigned>(type)];
  if (std::find(listeners.begin(), listeners.end(), listener)
      == listeners.end()) {
    listeners.emplace_back(listener);
  }
#endif
}

void ICanvas::addKeyEventListener(
  EventType /*type*/, const FastFunc<void(Event&& evt)>& /*listener*/,
  bool /*useCapture*/)
{
#if 0
  auto& listeners = keyEventListeners[static_cast<unsigned>(type)];
  if (std::find(listeners.begin(), listeners.end(), listener)
      == listeners.end()) {
    listeners.emplace_back(listener);
  }
#endif
}

void ICanvas::removeMouseEventListener(
  EventType /*type*/, const FastFunc<void(PointerEvent&& evt)>& /*listener*/)
{
#if 0
  auto& listeners = mouseEventListeners[static_cast<unsigned>(type)];
  listeners.erase(std::remove(listeners.begin(), listeners.end(), listener),
                  listeners.end());
#endif
}

void ICanvas::removeKeyEventListener(
  EventType /*type*/, const FastFunc<void(Event&& evt)>& /*listener*/)
{
#if 0
  auto& listeners = keyEventListeners[static_cast<unsigned>(type)];
  listeners.erase(std::remove(listeners.begin(), listeners.end(), listener),
                  listeners.end());
#endif
}

void ICanvas::setFrameSize(int w, int h)
{
  _renderingContext->drawingBufferWidth = width = w;
  _renderingContext->drawingBufferHeight = height = h;

  clientWidth  = w;
  clientHeight = h;

  _boundingClientRect.bottom = clientHeight;
  _boundingClientRect.height = clientHeight;
  _boundingClientRect.left   = 0;
  _boundingClientRect.right  = clientWidth;
  _boundingClientRect.top    = 0;
  _boundingClientRect.width  = clientWidth;
}

void ICanvas::onError(int errorId, const char* errorDesc)
{
  BABYLON_LOGF_ERROR("ICanvas", "Error occured: errorId = %d, message: %s",
                     errorId, errorDesc);
}

void ICanvas::onMouseButtonDown(int /*iMouseX*/, int /*iMouseY*/,
                                MouseButtonType /*btn*/)
{
#if 0
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_BUTTON_DOWN);
  for (auto& listener : mouseEventListeners[type]) {
    PointerEvent evt;
    evt.clientX = iMouseX;
    evt.clientY = iMouseY;
    evt.type    = EventType::MOUSE_BUTTON_DOWN;
    evt.button  = btn;
    listener(::std::move(evt));
  }
#endif
}

void ICanvas::onMouseButtonUp(int /*iMouseX*/, int /*iMouseY*/,
                              MouseButtonType /*btn*/)
{
#if 0
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_BUTTON_UP);
  for (auto& listener : mouseEventListeners[type]) {
    PointerEvent evt;
    evt.clientX = iMouseX;
    evt.clientY = iMouseY;
    evt.type    = EventType::MOUSE_BUTTON_UP;
    evt.button  = btn;
    listener(::std::move(evt));
  }
#endif
}

void ICanvas::onMouseMove(int /*iMouseX*/, int /*iMouseY*/)
{
#if 0
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_MOVE);
  for (auto& listener : mouseEventListeners[type]) {
    PointerEvent evt;
    evt.clientX = iMouseX;
    evt.clientY = iMouseY;
    evt.type    = EventType::MOUSE_MOVE;
    listener(::std::move(evt));
  }
#endif
}

void ICanvas::onMouseWheel(float /*wheelDelta*/)
{
#if 0
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_WHEEL);
  for (auto& listener : mouseEventListeners[type]) {
    PointerEvent evt;
    evt.type = EventType::MOUSE_WHEEL;
    listener(::std::move(evt));
  }
#endif
}

void ICanvas::onKeyDown(bool /*ctrlKey*/, int /*keyCode*/)
{
#if 0
  const unsigned int type = static_cast<unsigned>(EventType::KEY_DOWN);
  for (auto& listener : keyEventListeners[type]) {
    Event evt;
    listener(::std::move(evt));
  }
#endif
}

void ICanvas::onKeyUp(bool /*ctrlKey*/, int /*keyCode*/)
{
#if 0
  const unsigned int type = static_cast<unsigned>(EventType::KEY_UP);
  for (auto& listener : keyEventListeners[type]) {
    Event evt;
    listener(::std::move(evt));
  }
#endif
}

void ICanvas::focus()
{
}

void ICanvas::applicationDidEnterBackground()
{
}

void ICanvas::applicationWillEnterForeground()
{
}

void ICanvas::setPointerCapture(int /*pointerId*/)
{
}

void ICanvas::releasePointerCapture(int /*pointerId*/)
{
}

} // end of namespace BABYLON
