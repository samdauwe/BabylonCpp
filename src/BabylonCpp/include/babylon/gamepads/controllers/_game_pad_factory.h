#ifndef BABYLON_GAMEPADS_CONTROLLERS_GAME_PAD_FACTORY_H
#define BABYLON_GAMEPADS_CONTROLLERS_GAME_PAD_FACTORY_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class IBrowserGamepad;
class WebVRController;
using IBrowserGamepadPtr = std::shared_ptr<IBrowserGamepad>;
using WebVRControllerPtr = std::shared_ptr<WebVRController>;

struct _GamePadFactory {
  virtual ~_GamePadFactory() = default;

  /**
   * @brief Returns wether or not the current gamepad can be created for this
   * type of controller.
   * @param gamepadInfo Defines the gamepad info as receveid from the controller
   * APIs.
   * @returns true if it can be created, otherwise false
   */
  virtual bool canCreate(const IBrowserGamepadPtr& /*gamepadInfo*/) const
  {
    return false;
  }

  /**
   * @brief Creates a new instance of the Gamepad.
   * @param gamepadInfo Defines the gamepad info as receveid from the controller
   * APIs.
   * @returns the new gamepad instance
   */
  virtual WebVRControllerPtr
  create(const IBrowserGamepadPtr& /*gamepadInfo*/) const
  {
    return nullptr;
  }
}; // end of struct _GamePadFactory

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_CONTROLLERS_GAME_PAD_FACTORY_H
