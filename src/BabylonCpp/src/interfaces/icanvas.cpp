#include <babylon/interfaces/icanvas.h>

#include <babylon/babylon_stl_util.h>
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
  mouseEventListeners.resize(static_cast<unsigned>(EventType::UNKNOWN), {});
  keyEventListeners.resize(static_cast<unsigned>(EventType::UNKNOWN), {});
}

ICanvas::~ICanvas() = default;

void ICanvas::addMouseEventListener(
  EventType type, const std::function<void(PointerEvent&& evt)>& listener,
  bool /*useCapture*/)
{
  auto& listeners = mouseEventListeners[static_cast<unsigned>(type)];
  // Add listener
  if (listeners.empty()) {
    // Always add when listeners list is empty
    listeners.emplace_back(listener);
  }
  else {
#if 0
    // Add when listener is not yet in the listeners list
    auto it = std::find_if(
      listeners.begin(), listeners.end(),
      [&listener](
        const std::function<void(PointerEvent && evt)>& mouseEventListener) {
        return stl_util::get_address(listener)
               == stl_util::get_address(mouseEventListener);
      });
    if (it == listeners.end()) {
      listeners.emplace_back(listener);
    }
#else
    listeners.emplace_back(listener);
#endif
  }
}

void ICanvas::addKeyEventListener(
  EventType type, const std::function<void(KeyboardEvent&& evt)>& listener,
  bool /*useCapture*/)
{
  auto& listeners = keyEventListeners[static_cast<unsigned>(type)];
  // Add listener
  if (listeners.empty()) {
    // Always add when listeners list is empty
    listeners.emplace_back(listener);
  }
  else {
#if 0
    // Add when listener is not yet in the listeners list
    auto it = std::find_if(
      listeners.begin(), listeners.end(),
      [&listener](
        const std::function<void(KeyboardEvent && evt)>& keyEventListener) {
        return stl_util::get_address(listener)
               == stl_util::get_address(keyEventListener);
      });
    if (it == listeners.end()) {
      listeners.emplace_back(listener);
    }
#else
    listeners.emplace_back(listener);
#endif
  }
}

void ICanvas::removeMouseEventListener(
  EventType type, const std::function<void(PointerEvent&& evt)>& listener)
{
  auto& listeners = mouseEventListeners[static_cast<unsigned>(type)];
  // Remove from listeners
  listeners.erase(
    std::remove_if(
      listeners.begin(), listeners.end(),
      [&listener](
        const std::function<void(PointerEvent && evt)>& mouseEventListener) {
        return stl_util::get_address(listener)
               == stl_util::get_address(mouseEventListener);
      }),
    listeners.end());
}

void ICanvas::removeKeyEventListener(
  EventType type, const std::function<void(KeyboardEvent&& evt)>& listener)
{
  auto& listeners = keyEventListeners[static_cast<unsigned>(type)];
  // Remove from listeners
  listeners.erase(
    std::remove_if(
      listeners.begin(), listeners.end(),
      [&listener](
        const std::function<void(KeyboardEvent && evt)>& keyEventListener) {
        return stl_util::get_address(listener)
               == stl_util::get_address(keyEventListener);
      }),
    listeners.end());
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

void ICanvas::onMouseButtonDown(bool ctrlKey, bool shiftKey, int iMouseX,
                                int iMouseY, MouseButtonType btn)
{
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_BUTTON_DOWN);
  for (auto& listener : mouseEventListeners[type]) {
    PointerEvent evt;
    evt.clientX    = iMouseX;
    evt.clientY    = iMouseY;
    evt.movementX  = 0;
    evt.movementY  = 0;
    evt.type       = EventType::MOUSE_BUTTON_DOWN;
    evt.button     = btn;
    evt.buttons    = 1;
    evt.ctrlKey    = ctrlKey;
    evt.shiftKey   = shiftKey;
    evt.srcElement = this;
    listener(std::move(evt));
  }
}

void ICanvas::onMouseButtonUp(bool ctrlKey, bool shiftKey, int iMouseX,
                              int iMouseY, MouseButtonType btn)
{
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_BUTTON_UP);
  for (auto& listener : mouseEventListeners[type]) {
    PointerEvent evt;
    evt.clientX    = iMouseX;
    evt.clientY    = iMouseY;
    evt.movementX  = 0;
    evt.movementY  = 0;
    evt.type       = EventType::MOUSE_BUTTON_UP;
    evt.button     = btn;
    evt.buttons    = 1;
    evt.ctrlKey    = ctrlKey;
    evt.shiftKey   = shiftKey;
    evt.srcElement = this;
    listener(std::move(evt));
  }
}

void ICanvas::onMouseMove(bool ctrlKey, bool shiftKey, int iMouseX, int iMouseY)
{
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_MOVE);
  for (auto& listener : mouseEventListeners[type]) {
    PointerEvent evt;
    evt.clientX    = iMouseX;
    evt.clientY    = iMouseY;
    evt.movementX  = 0;
    evt.movementY  = 0;
    evt.type       = EventType::MOUSE_MOVE;
    evt.buttons    = 1;
    evt.ctrlKey    = ctrlKey;
    evt.shiftKey   = shiftKey;
    evt.srcElement = this;
    listener(std::move(evt));
  }
}

void ICanvas::onMouseWheel(bool ctrlKey, bool shiftKey, int iMouseX,
                           int iMouseY, float wheelDelta)
{
  const unsigned int type = static_cast<unsigned>(EventType::MOUSE_WHEEL);
  for (auto& listener : mouseEventListeners[type]) {
    MouseWheelEvent evt;
    evt.clientX    = iMouseX;
    evt.clientY    = iMouseY;
    evt.movementX  = 0;
    evt.movementY  = 0;
    evt.type       = EventType::MOUSE_WHEEL;
    evt.buttons    = 1;
    evt.ctrlKey    = ctrlKey;
    evt.shiftKey   = shiftKey;
    evt.wheelDelta = wheelDelta;
    evt.detail     = 0.f;
    evt.srcElement = this;
    listener(std::move(evt));
  }
}

void ICanvas::onKeyDown(bool ctrlKey, bool altKey, int keyCode,
                        std::string code)
{
  const unsigned int type = static_cast<unsigned>(EventType::KEY_DOWN);
  for (auto& listener : keyEventListeners[type]) {
    KeyboardEvent evt;
    evt.type    = EventType::KEY_DOWN;
    evt.ctrlKey = ctrlKey;
    evt.altKey  = altKey;
    evt.keyCode = keyCode;
    evt.code    = std::move(code);
    listener(std::move(evt));
  }
}

void ICanvas::onKeyUp(bool ctrlKey, bool altKey, int keyCode, std::string code)
{
  const unsigned int type = static_cast<unsigned>(EventType::KEY_UP);
  for (auto& listener : keyEventListeners[type]) {
    KeyboardEvent evt;
    evt.type    = EventType::KEY_UP;
    evt.ctrlKey = ctrlKey;
    evt.altKey  = altKey;
    evt.keyCode = keyCode;
    evt.code    = std::move(code);
    listener(std::move(evt));
  }
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
