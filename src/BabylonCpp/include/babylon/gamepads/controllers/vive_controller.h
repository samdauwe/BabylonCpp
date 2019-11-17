#ifndef BABYLON_GAMEPADS_CONTROLLERS_VIVE_CONTROLLER_H
#define BABYLON_GAMEPADS_CONTROLLERS_VIVE_CONTROLLER_H

#include <babylon/babylon_api.h>
#include <babylon/gamepads/controllers/_game_pad_factory.h>
#include <babylon/gamepads/controllers/web_vr_controller.h>

namespace BABYLON {

class ViveController;
using ViveControllerPtr = std::shared_ptr<ViveController>;

/**
 * @brief Vive Controller factory.
 */
struct ViveControllerFactory : public _GamePadFactory {
  /**
   * @brief Returns wether or not the current gamepad can be created for this
   * type of controller.
   * @param gamepadInfo Defines the gamepad info as receveid from the controller
   * APIs.
   * @returns true if it can be created, otherwise false
   */
  [[nodiscard]] bool canCreate(const IBrowserGamepadPtr& gamepadInfo) const override;

  /**
   * @brief Creates a new instance of the Gamepad.
   * @param gamepadInfo Defines the gamepad info as receveid from the controller
   * APIs.
   * @returns the new gamepad instance
   */
  [[nodiscard]] WebVRControllerPtr create(const IBrowserGamepadPtr& gamepadInfo) const override;
}; // end of struct ViveControllerFactory

/**
 * @brief Vive Controller.
 */
class BABYLON_SHARED_EXPORT ViveController : public WebVRController {

public:
  /**
   * Base Url for the controller model.
   */
  static constexpr const char* MODEL_BASE_URL = "https://controllers.babylonjs.com/vive/";
  /**
   * File name for the controller model.
   */
  static constexpr const char* MODEL_FILENAME = "wand.babylon";

public:
  template <typename... Ts>
  static ViveControllerPtr New(Ts&&... args)
  {
    return std::shared_ptr<ViveController>(new ViveController(std::forward<Ts>(args)...));
  }
  ~ViveController() override; // = default

  /**
   * @brief Implements abstract method on WebVRController class, loading
   * controller meshes and calling this.attachToMesh if successful.
   * @param scene scene in which to add meshes
   * @param meshLoaded optional callback function that will be called if the
   * mesh loads successfully.
   */
  void initControllerMesh(Scene* scene,
                          const std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

protected:
  /**
   * @brief Creates a new ViveController from a gamepad.
   * @param vrGamepad the gamepad that the controller should be created from
   */
  ViveController(const IBrowserGamepadPtr& vrGamepad);

  /**
   * @brief Fired when the left button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onLeftButtonStateChangedObservable();

  /**
   * @brief Fired when the right button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onRightButtonStateChangedObservable();

  /**
   * @brief Fired when the menu button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onMenuButtonStateChangedObservable();

  /**
   * @brief Called once for each button that changed state since the last frame.
   * Vive mapping:
   * 0: touchpad
   * 1: trigger
   * 2: left AND right buttons
   * 3: menu button
   * @param buttonIdx Which button index changed
   * @param state New state of the button
   * @param changes Which properties on the state changed since last frame
   */
  void _handleButtonChange(unsigned int buttonIdx, const ExtendedGamepadButton& state,
                           const GamepadButtonChanges& changes) override;

public:
  /**
   * Fired when the left button on this controller is modified.
   */
  ReadOnlyProperty<ViveController, Observable<ExtendedGamepadButton>>
    onLeftButtonStateChangedObservable;

  /**
   * Fired when the right button on this controller is modified.
   */
  ReadOnlyProperty<ViveController, Observable<ExtendedGamepadButton>>
    onRightButtonStateChangedObservable;

  /**
   * Fired when the menu button on this controller is modified.
   */
  ReadOnlyProperty<ViveController, Observable<ExtendedGamepadButton>>
    onMenuButtonStateChangedObservable;

}; // end of class ViveController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_CONTROLLERS_VIVE_CONTROLLER_H
