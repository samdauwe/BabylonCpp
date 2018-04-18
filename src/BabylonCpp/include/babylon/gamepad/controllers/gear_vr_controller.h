#ifndef BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

/**
 * @brief Gear VR Controller.
 */
class BABYLON_SHARED_EXPORT GearVRController : public WebVRController {

public:
  /**
   * Base Url for the controller model.
   */
  static constexpr const char* MODEL_BASE_URL
    = "https://controllers.babylonjs.com/generic/";
  /**
   * File name for the controller model.
   */
  static constexpr const char* MODEL_FILENAME = "generic.babylon";

  /**
   * Gamepad Id prefix used to identify this controller.
   */
  // id is 'Gear VR Controller'
  static constexpr const char* GAMEPAD_ID_PREFIX = "Gear VR";

public:
  /**
   * @brief Creates a new GearVRController from a gamepad.
   * @param vrGamepad the gamepad that the controller should be created from
   */
  GearVRController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
  ~GearVRController() override;

  /**
   * @brief Implements abstract method on WebVRController class, loading
   * controller meshes and calling this.attachToMesh if successful.
   * @param scene scene in which to add meshes
   * @param meshLoaded optional callback function that will be called if the
   * mesh loads successfully.
   */
  void initControllerMesh(
    Scene* scene,
    const ::std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

protected:
  /**
   * @brief Called once for each button that changed state since the last frame.
   * @param buttonIdx Which button index changed
   * @param state New state of the button
   * @param changes Which properties on the state changed since last frame
   */
  void _handleButtonChange(unsigned int buttonIdx,
                           const ExtendedGamepadButton& state,
                           const GamepadButtonChanges& changes) override;

private:
  const array_t<string_t, 2> _buttonIndexToObservableNameMap;

}; // end of class GearVRController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H
