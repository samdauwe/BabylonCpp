#ifndef BABYLON_GAMEPADS_CONTROLLERS_GAME_PAD_FACTORY_H
#define BABYLON_GAMEPADS_CONTROLLERS_GAME_PAD_FACTORY_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

FWD_CLASS_SPTR(IBrowserGamepad)
FWD_CLASS_SPTR(WebVRController)

struct _GamePadFactory {
  virtual ~_GamePadFactory() = default;

  /**
   * @brief Returns whether or not the current gamepad can be created for this type of controller.
   * @param gamepadInfo Defines the gamepad info as received from the controller APIs.
   * @returns true if it can be created, otherwise false
   */
  [[nodiscard]] virtual bool canCreate(const IBrowserGamepadPtr& /*gamepadInfo*/) const
  {
    return false;
  }

  /**
   * @brief Creates a new instance of the Gamepad.
   * @param gamepadInfo Defines the gamepad info as received from the controller APIs.
   * @returns the new gamepad instance
   */
  [[nodiscard]] virtual WebVRControllerPtr create(const IBrowserGamepadPtr& /*gamepadInfo*/) const
  {
    return nullptr;
  }
}; // end of struct _GamePadFactory

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_CONTROLLERS_GAME_PAD_FACTORY_H
