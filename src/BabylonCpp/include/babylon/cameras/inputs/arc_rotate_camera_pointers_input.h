#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_POINTERS_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_POINTERS_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

struct ArcRotateCameraPointer {
  int x;
  int y;
  int pointerId;
  PointerType type;
}; // end of struct ArcRotateCameraPointer

struct MultiTouchPanPosition {
  int x;
  int y;
  bool isPaning;
  bool isPinching;
}; // end of struct MultiTouchPanPosition

/**
 * @brief Manage the pointers inputs to control an arc rotate camera.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT ArcRotateCameraPointersInput
    : public ICameraInput<ArcRotateCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  ArcRotateCameraPointersInput();
  virtual ~ArcRotateCameraPointersInput();

  /**
   * @brief Attach the input controls to a specific dom element to get the input
   * from.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* canvas) override;

  /**
   * @brief Update the current camera state depending on the inputs that have
   * been used this frame. This is a dynamically created lambda to avoid the
   * performance penalty of looping for inputs in the render loop.
   */
  void checkInputs() override;

  /**
   * @brief Gets the class name of the current intput.
   * @returns the class name
   */
  const char* getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  const char* getSimpleName() const override;

public:
  /**
   * Defines the buttons associated with the input to handle camera move.
   */
  std::array<MouseButtonType, 3> buttons;

  /**
   * Defines the pointer angular sensibility  along the X axis or how fast is
   * the camera rotating.
   */
  float angularSensibilityX;

  /**
   * Defines the pointer angular sensibility along the Y axis or how fast is the
   * camera rotating.
   */
  float angularSensibilityY;

  /**
   * Defines the pointer pinch precision or how fast is the camera zooming.
   */
  float pinchPrecision;

  /**
   * pinchDeltaPercentage will be used instead of pinchPrecision if different
   * from 0.
   * It defines the percentage of current camera.radius to use as delta when
   * pinch zoom is used.
   */
  float pinchDeltaPercentage;

  /**
   * Defines the pointer panning sensibility or how fast is the camera moving.
   */
  float panningSensibility;

  /**
   * Defines whether panning (2 fingers swipe) is enabled through multitouch.
   */
  bool multiTouchPanning;

  /**
   * Defines whether panning is enabled for both pan (2 fingers swipe) and zoom
   * (pinch) through multitouch.
   */
  bool multiTouchPanAndZoom;

  /**
   * Revers pinch action direction.
   */
  bool pinchInwards;

private:
  ICanvas* _canvas;
  Engine* _engine;
  bool _noPreventDefault;
  bool _isPanClick;
  std::function<void(PointerInfo* p, EventState& es)> _pointerInput;
  Observer<PointerInfo>::Ptr _observer;
  std::function<void(KeyboardEvent& e)> _onKeyDown;
  std::function<void(KeyboardEvent& e)> _onKeyUp;
  std::function<void(MouseEvent& e)> _onMouseMove;
  std::function<void(GestureEvent& e)> _onGesture;
  std::function<void(FocusEvent& e)> _onLostFocus;
  std::function<void(PointerEvent&& e)> _onContextMenu;
  // pointers
  ArcRotateCameraPointer _cacheSoloPointer;
  ArcRotateCameraPointer _pointA;
  ArcRotateCameraPointer _pointB;
  bool _cacheSoloPointerDefined;
  bool _pointADefined;
  bool _pointBDefined;
  float _previousPinchSquaredDistance;
  float _initialDistance;
  unsigned int _twoFingerActivityCount;
  MultiTouchPanPosition _previousMultiTouchPanPosition;

}; // end of class ArcRotateCameraPointersInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_POINTERS_INPUT_H
