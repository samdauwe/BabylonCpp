#ifndef BABYLON_GAMEPAD_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_WINDOWS_MOTION_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/gamepad/controllers/web_vr_controller.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IMeshInfo {
  unsigned int index;
  AbstractMesh* value;
}; // end of struct IMeshInfo

struct BABYLON_SHARED_EXPORT IButtonMeshInfo : public IMeshInfo {
  AbstractMesh* pressed;
  AbstractMesh* unpressed;
}; // end of struct IButtonMeshInfo

struct BABYLON_SHARED_EXPORT IAxisMeshInfo : public IMeshInfo {
  AbstractMesh* min;
  AbstractMesh* max;
}; // end of struct IAxisMeshInfo

struct BABYLON_SHARED_EXPORT LoadedMeshInfo {
  AbstractMesh* rootNode;
  AbstractMesh* pointingPoseNode;
  unordered_map_t<string_t, Nullable<IButtonMeshInfo>> buttonMeshes;
  unordered_map_t<unsigned int, Nullable<IAxisMeshInfo>> axisMeshes;
}; // end of struct LoadedMeshInfo

class BABYLON_SHARED_EXPORT WindowsMotionController : public WebVRController {

public:
  static const string_t MODEL_BASE_URL;
  static const string_t MODEL_LEFT_FILENAME;
  static const string_t MODEL_RIGHT_FILENAME;

  static const string_t GAMEPAD_ID_PREFIX;
  static const string_t GAMEPAD_ID_PATTERN;

public:
  WindowsMotionController(const shared_ptr_t<IBrowserGamepad>& vrGamepad);
  ~WindowsMotionController() override;

  Observable<ExtendedGamepadButton>& onTriggerButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onMenuButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onGripButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onThumbstickButtonStateChangedObservable();
  Observable<ExtendedGamepadButton>& onTouchpadButtonStateChangedObservable();
  Observable<StickValues>& onTouchpadValuesChangedObservable();

  /**
   * Called once per frame by the engine.
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

  void lerpButtonTransform(const string_t& buttonName, float buttonValue);
  void lerpAxisTransform(unsigned int axis, float axisValue);
  Ray getForwardRay(float length = 100.f);
  void dispose() override;

private:
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
  Observable<StickValues> onTrackpadValuesChangedObservable;
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
