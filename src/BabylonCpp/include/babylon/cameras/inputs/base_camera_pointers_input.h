#ifndef BABYLON_CAMERAS_INPUTS_BASE_CAMERA_POINTERS_INPUT_H
#define BABYLON_CAMERAS_INPUTS_BASE_CAMERA_POINTERS_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

/**
 * @brief Base class for Camera Pointer Inputs.
 * See FollowCameraPointersInput in
 * src/Cameras/Inputs/followCameraPointersInput.ts for example usage.
 */
template <class TCamera>
class BABYLON_SHARED_EXPORT BaseCameraPointersInput : public ICameraInput<TCamera> {

public:
  BaseCameraPointersInput();
  ~BaseCameraPointersInput() override; // = default

  /**
   * @brief Attach the input controls to a specific dom element to get the input
   * from.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* element, bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* element) override;

  /**
   * @brief Update the current camera state depending on the inputs that have
   * been used this frame. This is a dynamically created lambda to avoid the
   * performance penalty of looping for inputs in the render loop.
   */
  void checkInputs() override;

  /**
   * @brief Gets the class name of the current input.
   * @returns the class name
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  [[nodiscard]] std::string getSimpleName() const override;

protected:
  /**
   * @brief Called on pointer POINTERDOUBLETAP event.
   * Override this method to provide functionality on POINTERDOUBLETAP event.
   */
  virtual void onDoubleTap(const std::string& type);

  /**
   * @brief Called on pointer POINTERMOVE event if only a single touch is
   * active. Override this method to provide functionality.
   */
  virtual void onTouch(const std::optional<PointerTouch>& point, int offsetX, int offsetY);

  /**
   * @brief Called on pointer POINTERMOVE event if multiple touches are active.
   * Override this method to provide functionality.
   */
  virtual void onMultiTouch(const std::optional<PointerTouch>& pointA,
                            const std::optional<PointerTouch>& pointB,
                            float previousPinchSquaredDistance, float pinchSquaredDistance,
                            const std::optional<PointerTouch>& previousMultiTouchPanPosition,
                            const std::optional<PointerTouch>& multiTouchPanPosition);

  /**
   * @brief Called on JS contextmenu event.
   * Override this method to provide functionality.
   */
  virtual void onContextMenu(PointerEvent& evt);

  /**
   * @brief Called each time a new POINTERDOWN event occurs. Ie, for each button
   * press.
   * Override this method to provide functionality.
   */
  virtual void onButtonDown(PointerEvent& evt);

  /**
   * @brief Called each time a new POINTERUP event occurs. Ie, for each button
   * release.
   * Override this method to provide functionality.
   */
  virtual void onButtonUp(PointerEvent& evt);

  /**
   * @brief Called when window becomes inactive.
   * Override this method to provide functionality.
   */
  virtual void onLostFocus();

public:
  /**
   * Defines the buttons associated with the input to handle camera move.
   */
  std::array<MouseButtonType, 3> buttons;

protected:
  /**
   * Whether keyboard modifier keys are pressed at time of last mouse event.
   */
  bool _altKey;
  bool _ctrlKey;
  bool _metaKey;
  bool _shiftKey;

  /**
   * Which mouse buttons were pressed at time of last mouse event.
   * https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/buttons
   */
  MouseButtonType _buttonsPressed;

private:
  std::function<void(PointerInfo* p, EventState& es)> _pointerInput;
  Observer<PointerInfo>::Ptr _observer;
  std::function<void(FocusEvent& e)> _onLostFocus;
  std::optional<PointerTouch> pointA;
  std::optional<PointerTouch> pointB;
  ICanvas* _element;
  bool _noPreventDefault;
  Engine* _engine;
  float _previousPinchSquaredDistance;
  std::optional<PointerTouch> _previousMultiTouchPanPosition;

}; // end of class BaseCameraPointersInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_BASE_CAMERA_POINTERS_INPUT_H
