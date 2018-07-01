#ifndef BABYLON_GAMEPAD_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

/**
 * Defines the IMeshInfo object that describes information a webvr controller
 * mesh
 */
struct BABYLON_SHARED_EXPORT IMeshInfo {
  /**
   * Index of the mesh inside the root mesh
   */
  unsigned int index = 0;
  /**
   * The mesh
   */
  AbstractMesh* value = nullptr;
}; // end of struct IMeshInfo

/**
 * Defines the IButtonMeshInfo object that describes a button mesh
 */
struct BABYLON_SHARED_EXPORT IButtonMeshInfo : public IMeshInfo {
  /**
   * The mesh that should be displayed when pressed
   */
  AbstractMesh* pressed = nullptr;
  /**
   * The mesh that should be displayed when not pressed
   */
  AbstractMesh* unpressed = nullptr;
}; // end of struct IButtonMeshInfo

/**
 * Defines the IAxisMeshInfo object that describes an axis mesh
 */
struct BABYLON_SHARED_EXPORT IAxisMeshInfo : public IMeshInfo {
  /**
   * The mesh that should be set when at its min
   */
  AbstractMesh* min = nullptr;
  /**
   * The mesh that should be set when at its max
   */
  AbstractMesh* max = nullptr;
}; // end of struct IAxisMeshInfo

/**
 * Defines the LoadedMeshInfo object that describes information about the loaded
 * webVR controller mesh
 */
struct BABYLON_SHARED_EXPORT LoadedMeshInfo {
  /**
   * Root of the mesh
   */
  AbstractMesh* rootNode = nullptr;
  /**
   * Node of the mesh corrisponding to the direction the ray should be cast from
   * the controller
   */
  AbstractMesh* pointingPoseNode = nullptr;
  /**
   * Map of the button meshes contained in the controller
   */
  unordered_map_t<string_t, Nullable<IButtonMeshInfo>> buttonMeshes;
  /**
   * Map of the axis meshes contained in the controller
   */
  unordered_map_t<unsigned int, Nullable<IAxisMeshInfo>> axisMeshes;
}; // end of struct LoadedMeshInfo

/**
 * Defines the WindowsMotionController object that the state of the windows
 * motion controller
 */
class BABYLON_SHARED_EXPORT WindowsMotionController : public WebVRController {

public:
  /**
   * The base url used to load the left and right controller models
   */
  static const string_t MODEL_BASE_URL;
  /**
   * The name of the left controller model file
   */
  static const string_t MODEL_LEFT_FILENAME;
  /**
   * The name of the right controller model file
   */
  static const string_t MODEL_RIGHT_FILENAME;

  /**
   * The controller name prefix for this controller type
   */
  static const string_t GAMEPAD_ID_PREFIX;
  /**
   * The controller id pattern for this controller type
   */
  static const string_t GAMEPAD_ID_PATTERN;

public:
  /**
   * Creates a new WindowsMotionController from a gamepad.
   * @param vrGamepad the gamepad that the controller should be created from
   */
  WindowsMotionController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
  ~WindowsMotionController() override;

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
   * @brief Called once for each button that changed state since the last frame
   * @param buttonIdx Which button index changed
   * @param state New state of the button
   * @param changes Which properties on the state changed since last frame
   */
  void _handleButtonChange(unsigned int buttonIdx,
                           const ExtendedGamepadButton& state,
                           const GamepadButtonChanges& changes) override;

  /**
   * @brief Moves the buttons on the controller mesh based on their current
   * state.
   * @param buttonName the name of the button to move
   * @param buttonValue the value of the button which determines the buttons new
   * position
   */
  void _lerpButtonTransform(const string_t& buttonName, float buttonValue);

  /**
   * @brief Moves the axis on the controller mesh based on its current state
   * @param axis the index of the axis
   * @param axisValue the value of the axis which determines the meshes new
   * position
   */
  void _lerpAxisTransform(unsigned int axis, float axisValue);

  /**
   * @brief Gets the ray of the controller in the direction the controller is
   * pointing.
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
  Observable<ExtendedGamepadButton>&
  get_onTriggerButtonStateChangedObservable();

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
  Observable<ExtendedGamepadButton>&
  get_onThumbstickButtonStateChangedObservable();

  /**
   * @brief Fired when the touchpad button on this controller is modified.
   */
  Observable<ExtendedGamepadButton>&
  get_onTouchpadButtonStateChangedObservable();

  /**
   * @brief Fired when the touchpad values on this controller are modified.
   */
  Observable<StickValues>& get_onTouchpadValuesChangedObservable();

private:
  void _updateTrackpad();

  /**
   * @brief Takes a list of meshes (as loaded from the glTF file) and finds the
   * root node, as well as nodes that can be transformed by button presses and
   * axes values, based on this._mapping.
   *
   * @param scene scene in which the meshes exist
   * @param meshes list of meshes that make up the controller model to process
   * @return structured view of the given meshes, with mapping of buttons and
   * axes to meshes that can be transformed.
   */
  LoadedMeshInfo processModel(Scene* scene,
                              const vector_t<AbstractMesh*>& meshes);

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
  Nullable<LoadedMeshInfo> _loadedMeshInfo;
  // Mappings

  // Semantic button names
  vector_t<string_t> _mappingButtons;

  // A mapping of the button name to glTF model node name
  // that should be transformed by button value.
  unordered_map_t<string_t, string_t> _mappingButtonMeshNames;

  // This mapping is used to translate from the Motion Controller to Babylon
  // semantics
  unordered_map_t<string_t, string_t> _mappingButtonObservableNames;

  // A mapping of the axis name to glTF model node name
  // that should be transformed by axis value.
  // This array mirrors the browserGamepad.axes array, such that
  // the mesh corresponding to axis 0 is in this array index 0.
  vector_t<string_t> _mappingAxisMeshNames;
  string_t _mappingPointingPoseMeshName;

}; // end of class WindowsMotionController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H
