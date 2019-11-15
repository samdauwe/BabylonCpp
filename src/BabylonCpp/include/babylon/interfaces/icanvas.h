#ifndef BABYLON_INTERFACES_ICANVAS_H
#define BABYLON_INTERFACES_ICANVAS_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/engines/engine_options.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

class ICanvasRenderingContext2D;

class BABYLON_SHARED_EXPORT ICanvas {

public:
  ICanvas();
  virtual ~ICanvas(); // = default
  virtual ClientRect& getBoundingClientRect()       = 0;
  virtual bool onlyRenderBoundingClientRect() const = 0;
  virtual bool initializeContext3d()                = 0;
  virtual ICanvasRenderingContext2D* getContext2d() = 0;
  virtual GL::IGLRenderingContext* getContext3d(const EngineOptions& options)
    = 0;
  // Event listeners
  void
  addMouseEventListener(EventType type,
                        const std::function<void(PointerEvent&& evt)>& listener,
                        bool useCapture = false);
  void
  addKeyEventListener(EventType type,
                      const std::function<void(KeyboardEvent&& evt)>& listener,
                      bool useCapture = false);
  void removeMouseEventListener(
    EventType type, const std::function<void(PointerEvent&& evt)>& listener);
  void removeKeyEventListener(
    EventType type, const std::function<void(KeyboardEvent&& evt)>& listener);

  void setFrameSize(int width, int height);
  void onError(int errorId, const char* errorDesc);
  // User unput handling
  void onMouseButtonDown(bool ctrlKey, bool shiftKey, int mouseX, int mouseY,
                         MouseButtonType btn);
  void onMouseButtonUp(bool ctrlKey, bool shiftKey, int mouseX, int mouseY,
                       MouseButtonType btn);
  void onMouseMove(bool ctrlKey, bool shiftKey, int mouseX, int mouseY);
  void onMouseWheel(bool ctrlKey, bool shiftKey, int mouseX, int mouseY,
                    float wheelDelta);
  void onKeyDown(bool ctrlKey, bool altKey, int keyCode, std::string code);
  void onKeyUp(bool ctrlKey, bool altKey, int keyCode, std::string code);

  void focus();
  void applicationDidEnterBackground();
  void applicationWillEnterForeground();
  void setPointerCapture(int pointerId);
  void releasePointerCapture(int pointerId);

public:
  int width;
  int height;
  int clientWidth;
  int clientHeight;
  int mouseX;
  int mouseY;
  int tabIndex;
  Style style;
  ClientRect _boundingClientRect;

protected:
  std::unique_ptr<GL::IGLRenderingContext> _renderingContext;
  std::vector<std::vector<std::function<void(PointerEvent&& evt)>>>
    mouseEventListeners;
  std::vector<std::vector<std::function<void(KeyboardEvent&& evt)>>>
    keyEventListeners;

protected:
  bool _initialized;

}; // end of class ICanvas

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_ICANVAS_H
