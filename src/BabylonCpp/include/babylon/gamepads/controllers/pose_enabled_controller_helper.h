#ifndef BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H
#define BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class IBrowserGamepad;
class WebVRController;

/**
 * @brief Defines the PoseEnabledControllerHelper object that is used initialize
 * a gamepad as the controller type it is specified as (eg. windows mixed
 * reality controller).
 */
struct BABYLON_SHARED_EXPORT PoseEnabledControllerHelper {

  /**
   * @brief Initializes a gamepad as the controller type it is specified as (eg.
   * windows mixed reality controller).
   * @param vrGamepad the gamepad to initialized
   * @returns a vr controller of the type the gamepad identified as
   */
  static WebVRController*
  InitiateController(const std::shared_ptr<IBrowserGamepad>& vrGamepad);

}; // end of struct PoseEnabledControllerHelper

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_HELPER_H
