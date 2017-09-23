#ifndef BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_INPUT_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FreeCameraMouseInput
  : public ICameraInput<FreeCamera> {

public:
  FreeCameraMouseInput(bool touchEnabled = true);
  virtual ~FreeCameraMouseInput();

  void attachControl(ICanvas* canvas, bool noPreventDefault) override;
  void detachControl(ICanvas* canvas) override;
  void checkInputs() override;
  const char* getClassName() const override;
  const char* getSimpleName() const override;

public:
  array_t<MouseButtonType, 3> buttons;
  float angularSensibility;
  bool touchEnabled;

private:
  ICanvas* _canvas;
  Engine* _engine;
  ::std::function<void(PointerInfo* p, EventState& es)> _pointerInput;
  ::std::function<void(MouseEvent& e)> _onMouseMove;
  Observer<PointerInfo>::Ptr _observer;
  PositionCoord _previousPosition;
  bool _previousPositionDefined;
  bool _noPreventDefault;

}; // end of class FreeCameraMouseInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_INPUT_H
