#ifndef BABYLON_GAMEPADS_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H
#define BABYLON_GAMEPADS_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H

#include <memory>
#include <optional>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/gamepads/controllers/_game_pad_factory.h>

namespace BABYLON {

class IBrowserGamepad;
class WebVRController;
using IBrowserGamepadPtr = std::shared_ptr<IBrowserGamepad>;
using WebVRControllerPtr = std::shared_ptr<WebVRController>;

/**
 * @brief Defines the PoseEnabledControllerHelper object that is used initialize a gamepad as the
 * controller type it is specified as (eg. windows mixed reality controller).
 */
struct BABYLON_SHARED_EXPORT PoseEnabledControllerHelper {

  /** Hidden */
  static std::vector<_GamePadFactory> _ControllerFactories;

  /** Hidden */
  static std::optional<_GamePadFactory> _DefaultControllerFactory;

  /**
   * @brief Initializes a gamepad as the controller type it is specified as (eg. windows mixed
   * reality controller).
   * @param vrGamepad the gamepad to initialized
   * @returns a vr controller of the type the gamepad identified as
   */
  static WebVRControllerPtr InitiateController(const IBrowserGamepadPtr& vrGamepad);

}; // end of struct PoseEnabledControllerHelper

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H
