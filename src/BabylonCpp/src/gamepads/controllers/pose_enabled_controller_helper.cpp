#include <babylon/gamepads/controllers/pose_enabled_controller_helper.h>

#include <babylon/gamepads/controllers/daydream_controller.h>
#include <babylon/gamepads/controllers/gear_vr_controller.h>
#include <babylon/gamepads/controllers/generic_controller.h>
#include <babylon/gamepads/controllers/oculus_touch_controller.h>
#include <babylon/gamepads/controllers/vive_controller.h>
#include <babylon/gamepads/controllers/windows_motion_controller.h>

namespace BABYLON {

std::vector<_GamePadFactory> PoseEnabledControllerHelper::_ControllerFactories{
  // Google Daydream Controller
  DaydreamControllerFactory(), //
  // Gear VR Controller
  GearVRControllerFactory(), //
  // Oculus Touch Controller
  OculusTouchControllerFactory(), //
  // Vive Controller
  ViveControllerFactory(), //
  // Windows motion Controller
  WindowsMotionControllerFactory() //
};

std::optional<_GamePadFactory>
  PoseEnabledControllerHelper::_DefaultControllerFactory
  = GenericControllerFactory();

WebVRControllerPtr PoseEnabledControllerHelper::InitiateController(
  const IBrowserGamepadPtr& vrGamepad)
{
  for (const auto& factory : _ControllerFactories) {
    if (factory.canCreate(vrGamepad)) {
      return factory.create(vrGamepad);
    }
  }

  if (_DefaultControllerFactory.has_value()) {
    return _DefaultControllerFactory->create(vrGamepad);
  }

  throw std::runtime_error(
    "The type of gamepad you are trying to load needs to be imported first or "
    "is not supported.");
}

} // end of namespace BABYLON
