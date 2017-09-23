#ifndef BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H
#define BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ArcRotateCameraMouseWheelInput
  : public ICameraInput<ArcRotateCamera> {

public:
  ArcRotateCameraMouseWheelInput();
  virtual ~ArcRotateCameraMouseWheelInput();

  void attachControl(ICanvas* canvas, bool noPreventDefault = false) override;
  void detachControl(ICanvas* canvas) override;
  void checkInputs() override;
  const char* getClassName() const override;
  const char* getSimpleName() const override;

public:
  float wheelPrecision;

private:
  ICanvas* _canvas;
  bool _noPreventDefault;
  ::std::function<void(PointerInfo* p, EventState& es)> _wheel;
  Observer<PointerInfo>::Ptr _observer;

}; // end of class ArcRotateCameraMouseWheelInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_MOUSE_WHEEL_INPUT_H
