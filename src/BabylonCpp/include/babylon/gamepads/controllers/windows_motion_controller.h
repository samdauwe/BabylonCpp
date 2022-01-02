#ifndef BABYLON_GAMEPADS_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H
#define BABYLON_GAMEPADS_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/gamepads/controllers/_game_pad_factory.h>
#include <babylon/gamepads/controllers/web_vr_controller.h>

namespace BABYLON {

class TransformNode;
class WindowsMotionController;
using WindowsMotionControllerPtr = std::shared_ptr<WindowsMotionController>;

/**
 * @brief Defines the IMeshInfo object that describes information a webvr controller mesh.
 */
struct BABYLON_SHARED_EXPORT IMeshInfo {
  /**
   * Index of the mesh inside the root mesh
   */
  unsigned int index = 0;
  /**
   * The mesh
   */
  TransformNode* value = nullptr;
}; // end of struct IMeshInfo

/**
 * @brief Defines the IButtonMeshInfo object that describes a button mesh.
 */
struct BABYLON_SHARED_EXPORT IButtonMeshInfo : public IMeshInfo {
  /**
   * The mesh that should be displayed when pressed
   */
  TransformNode* pressed = nullptr;
  /**
   * The mesh that should be displayed when not pressed
   */
  TransformNode* unpressed = nullptr;
}; // end of struct IButtonMeshInfo

/**
 * @brief Defines the IAxisMeshInfo object that describes an axis mesh.
 */
struct BABYLON_SHARED_EXPORT IAxisMeshInfo : public IMeshInfo {
  /**
   * The mesh that should be set when at its min
   */
  TransformNode* min = nullptr;
  /**
   * The mesh that should be set when at its max
   */
  TransformNode* max = nullptr;
}; // end of struct IAxisMeshInfo

/**
 * @brief Defines the LoadedMeshInfo object that describes information about the loaded webVR
 * controller mesh
 */
struct BABYLON_SHARED_EXPORT LoadedMeshInfo {
  /**
   * Root of the mesh
   */
  AbstractMesh* rootNode = nullptr;
  /**
   * Node of the mesh corresponding to the direction the ray should be cast from the controller
   */
  AbstractMesh* pointingPoseNode = nullptr;
  /**
   * Map of the button meshes contained in the controller
   */
  std::unordered_map<std::string, std::optional<IButtonMeshInfo>> buttonMeshes;
  /**
   * Map of the axis meshes contained in the controller
   */
  std::unordered_map<unsigned int, std::optional<IAxisMeshInfo>> axisMeshes;
}; // end of struct LoadedMeshInfo

/**
 * @brief Windows motion Controller factory.
 */
struct WindowsMotionControllerFactory : public _GamePadFactory {
  /**
   * @brief Returns wether or not the current gamepad can be created for this type of controller.
   * @param gamepadInfo Defines the gamepad info as receveid from the controller APIs.
   * @returns true if it can be created, otherwise false
   */
  [[nodiscard]] bool canCreate(const IBrowserGamepadPtr& gamepadInfo) const override;

  /**
   * @brief Creates a new instance of the Gamepad.
   * @param gamepadInfo Defines the gamepad info as receveid from the controller APIs.
   * @returns the new gamepad instance
   */
  [[nodiscard]] WebVRControllerPtr create(const IBrowserGamepadPtr& gamepadInfo) const override;
}; // end of struct WindowsMotionControllerFactory

/**
 * @brief Defines the WindowsMotionController object that the state of the windows motion controller
 */
class BABYLON_SHARED_EXPORT WindowsMotionController : public WebVRController {

public:
  /**
   * The base url used to load the left and right controller models
   */
  static const std::string MODEL_BASE_URL;
  /**
   * The name of the left controller model file
   */
  static const std::string MODEL_LEFT_FILENAME;
  /**
   * The name of the right controller model file
   */
  static const std::string MODEL_RIGHT_FILENAME;

  /**
   * The controller name prefix for this controller type
   */
  static const std::string GAMEPAD_ID_PREFIX;
  /**
   * The controller id pattern for this controller type
   */
  static const std::string GAMEPAD_ID_PATTERN;

public:
  template <typename... Ts>
  static WindowsMotionControllerPtr New(Ts&&... args)
  {
    return std::shared_ptr<WindowsMotionController>(
      new WindowsMotionController(std::forward<Ts>(args)...));
  }
  ~WindowsMotionController() override; // = default

  /**
   * Fired when the trigger on this controller is modified.
   */
  ReadOnlyProperty<WindowsMotionController, Observable<ExtendedGamepadButton>>
    onTriggerButtonStateChangedObservable;

  /**
   *  Fired when the menu button on this controller is modified.
   */
  ReadOnlyProperty<WindowsMotionController, Observable<ExtendedGamepadButton>>
    onMenuButtonStateChangedObservable;

  /**
   * Fired when the grip button on this controller is modified.
   */
  ReadOnlyProperty<WindowsMotionController, Observable<ExtendedGamepadButton>>
    onGripButtonStateChangedObservable;

  /**
   * Fired when the thumbstick button on this controller is modified.
   */
  ReadOnlyProperty<WindowsMotionController, Observable<ExtendedGamepadButton>>
    onThumbstickButtonStateChangedObservable;

  /**
   * Fired when the touchpad button on this controller is modified.
   */
  ReadOnlyProperty<WindowsMotionController, Observable<ExtendedGamepadButton>>
    onTouchpadButtonStateChangedObservable;

  /**
   * Fired when the touchpad values on this controller are modified.
   */
  ReadOnlyProperty<WindowsMotionController, Observable<StickValues>>
    onTouchpadValuesChangedObservable;

  /**
   * @brief Called once per frame by the engine.
   */
  void update() override;

  /**
   * @brief Implements abstract method on WebVRController class, loading controller meshes and
   * calling this.attachToMesh if successful.
   * @param scene scene in which to add meshes
   * @param meshLoaded optional callback function that will be called if the mesh loads
   * successfully.
   */
  void initControllerMesh(Scene* scene,
                          const std::function<void(AbstractMesh* mesh)>& meshLoaded) override;

protected:
  /**
   * @brief Creates a new WindowsMotionController from a gamepad.
   * @param vrGamepad the gamepad that the controller should be created from
   */
  WindowsMotionController(const IBrowserGamepadPtr& vrGamepad);

  /**
   * @brief Called once for each button that changed state since the last frame
   * @param buttonIdx Which button index changed
   * @param state New state of the button
   * @param changes Which properties on the state changed since last frame
   */
  void _handleButtonChange(unsigned int buttonIdx, const ExtendedGamepadButton& state,
                           const GamepadButtonChanges& changes) override;

  /**
   * @brief Moves the buttons on the controller mesh based on their current state.
   * @param buttonName the name of the button to move
   * @param buttonValue the value of the button which determines the buttons new position
   */
  void _lerpButtonTransform(const std::string& buttonName, float buttonValue);

  /**
   * @brief Moves the axis on the controller mesh based on its current state
   * @param axis the index of the axis
   * @param axisValue the value of the axis which determines the meshes new position
   */
  void _lerpAxisTransform(unsigned int axis, float axisValue);

  /**
   * @brief Gets the ray of the controller in the direction the controller is pointing.
   * @param length the length the resulting ray should be
   * @returns a ray in the direction the controller is pointing
   */
  Ray getForwardRay(float length = 100.f);

  /**
   * @brief Disposes of the controller.
   */
  void dispose() override;

protected:
  /**
   * @brief Fired when the trigger on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onTriggerButtonStateChangedObservable();

  /**
   * @brief Fired when the menu button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onMenuButtonStateChangedObservable();

  /**
   * @brief Fired when the grip button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onGripButtonStateChangedObservable();

  /**
   * @brief Fired when the thumbstick button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onThumbstickButtonStateChangedObservable();

  /**
   * @brief Fired when the touchpad button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>& get_onTouchpadButtonStateChangedObservable();

  /**
   * @brief Fired when the touchpad values on this controller are modified.
   */
  Observable<StickValues>& get_onTouchpadValuesChangedObservable();

  /**
   * @brief Hidden
   */
  void _updateTrackpad();

private:
  /**
   * @brief Takes a list of meshes (as loaded from the glTF file) and finds the root node, as well
   * as nodes that can be transformed by button presses and axes values, based on this._mapping.
   *
   * @param scene scene in which the meshes exist
   * @param meshes list of meshes that make up the controller model to process
   * @return structured view of the given meshes, with mapping of buttons and axes to meshes that
   * can be transformed.
   */
  LoadedMeshInfo processModel(Scene* scene, const std::vector<AbstractMesh*>& meshes);

  LoadedMeshInfo createMeshInfo(AbstractMesh* rootNode);

public:
  /**
   * Fired when the trackpad on this controller is modified
   */
  Observable<StickValues> onTrackpadValuesChangedObservable;

  /**
   * The current x and y values of this controller's trackpad
   */
  StickValues trackpad;

private:
  std::optional<LoadedMeshInfo> _loadedMeshInfo;
  // Mappings

  // Semantic button names
  std::vector<std::string> _mappingButtons;

  // A mapping of the button name to glTF model node name that should be transformed by button
  // value.
  std::unordered_map<std::string, std::string> _mappingButtonMeshNames;

  // This mapping is used to translate from the Motion Controller to Babylon semantics
  std::unordered_map<std::string, std::string> _mappingButtonObservableNames;

  // A mapping of the axis name to glTF model node name that should be transformed by axis value.
  // This array mirrors the browserGamepad.axes array, such that the mesh corresponding to axis 0 is
  // in this array index 0.
  std::vector<std::string> _mappingAxisMeshNames;
  std::string _mappingPointingPoseMeshName;

}; // end of class WindowsMotionController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H
