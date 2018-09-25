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

class BABYLON_SHARED_EXPORT ArcRotateCameraPointersInput
    : public ICameraInput<ArcRotateCamera> {

public:
  ArcRotateCameraPointersInput();
  virtual ~ArcRotateCameraPointersInput();

  void attachControl(ICanvas* canvas, bool noPreventDefault = false) override;
  void detachControl(ICanvas* canvas) override;
  void checkInputs() override;
  const char* getClassName() const override;
  const char* getSimpleName() const override;

public:
  std::array<MouseButtonType, 3> buttons;
  float angularSensibilityX;
  float angularSensibilityY;
  float pinchPrecision;

  /**
   * pinchDeltaPercentage will be used instead of pinchPrecision if different
   * from 0.
   * It defines the percentage of current camera.radius to use as delta when
   * pinch zoom is used.
   */
  float pinchDeltaPercentage;

  float panningSensibility;
  bool multiTouchPanning;
  bool multiTouchPanAndZoom;
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
