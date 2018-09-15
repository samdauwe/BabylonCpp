#ifndef BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H

#include <babylon/babylon_api.h>
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
  GearVRController(const std::shared_ptr<IBrowserGamepad>& vrGamepad);
  ~GearVRController() override;

  /**
   * @brief Updates the state of the pose enbaled controller based on the raw
   * pose data from the device
   * @param poseData raw pose fromthe device
   */
  void updateFromDevice(const DevicePose& poseData) override;

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
  float _maxRotationDistFromHeadset;
  float _draggedRoomRotation;
  Vector3 _tmpVector;
  const std::array<std::string, 2> _buttonIndexToObservableNameMap;

}; // end of class GearVRController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_GEAR_VR_CONTROLLER_H
