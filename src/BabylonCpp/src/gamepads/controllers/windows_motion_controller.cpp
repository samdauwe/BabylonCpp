#include <babylon/gamepads/controllers/windows_motion_controller.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/culling/ray.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

const std::string WindowsMotionController::MODEL_BASE_URL
  = "https://controllers.babylonjs.com/microsoft/";
const std::string WindowsMotionController::MODEL_LEFT_FILENAME  = "left.glb";
const std::string WindowsMotionController::MODEL_RIGHT_FILENAME = "right.glb";

const std::string WindowsMotionController::GAMEPAD_ID_PREFIX
  = "Spatial Controller (Spatial Interaction Source) ";
const std::string WindowsMotionController::GAMEPAD_ID_PATTERN = "([0-9a-zA-Z]+-[0-9a-zA-Z]+)";

bool WindowsMotionControllerFactory::canCreate(const IBrowserGamepadPtr& gamepadInfo) const
{
  return String::startsWith(gamepadInfo->id, WindowsMotionController::GAMEPAD_ID_PREFIX);
}

WebVRControllerPtr
WindowsMotionControllerFactory::create(const IBrowserGamepadPtr& gamepadInfo) const
{
  return WindowsMotionController::New(gamepadInfo);
}

WindowsMotionController::WindowsMotionController(
  const IBrowserGamepadPtr& vrGamepad)
    : WebVRController{vrGamepad}
    , onTriggerButtonStateChangedObservable{this,
         &WindowsMotionController::get_onTriggerButtonStateChangedObservable}
    , onMenuButtonStateChangedObservable{this,
        &WindowsMotionController::get_onMenuButtonStateChangedObservable}
    , onGripButtonStateChangedObservable{this,
        &WindowsMotionController::get_onGripButtonStateChangedObservable}
    , onThumbstickButtonStateChangedObservable{this,
        &WindowsMotionController::get_onThumbstickButtonStateChangedObservable}
    , onTouchpadButtonStateChangedObservable{this,
        &WindowsMotionController::get_onTouchpadButtonStateChangedObservable}
    , onTouchpadValuesChangedObservable{this,
        &WindowsMotionController::get_onTouchpadValuesChangedObservable}
    , trackpad{0.f,0.f}
    , _loadedMeshInfo{std::nullopt}
    // Semantic button names
    ,_mappingButtons{"thumbstick", "trigger", "grip", "menu", "trackpad"}
    // trigger, grip, trackpad, thumbstick, menu
    // A mapping of the button name to glTF model node name
    // that should be transformed by button value.
    , _mappingButtonMeshNames{              //
        {"trigger", "SELECT"},              //
        {"menu", "MENU"},                   //
        {"grip", "GRASP"},                  //
        {"thumbstick", "THUMBSTICK_PRESS"}, //
        {"trackpad", "TOUCHPAD_PRESS"}      //
    }                                       //
    // This mapping is used to translate from the Motion Controller to Babylon semantics
    ,_mappingButtonObservableNames {                         //
        {"trigger", "onTriggerStateChangedObservable"},      //
        {"menu", "onSecondaryButtonStateChangedObservable"}, //
        {"grip", "onMainButtonStateChangedObservable"},      //
        {"thumbstick", "onPadStateChangedObservable"},       //
        {"trackpad", "onTrackpadChangedObservable"}          //
    }                                                        //
    // A mapping of the axis name to glTF model node name
    // that should be transformed by axis value.
    // This array mirrors the browserGamepad.axes array, such that
    // the mesh corresponding to axis 0 is in this array index 0.
    ,_mappingAxisMeshNames{ //
        "THUMBSTICK_X",     //
        "THUMBSTICK_Y",     //
        "TOUCHPAD_TOUCH_X", //
        "TOUCHPAD_TOUCH_Y"  //
    }                       //
    // upside down in webxr
    ,_mappingPointingPoseMeshName{ PoseEnabledController::POINTING_POSE}

{
  controllerType = PoseEnabledControllerType::WINDOWS;
}

WindowsMotionController::~WindowsMotionController() = default;

Observable<ExtendedGamepadButton>&
WindowsMotionController::get_onTriggerButtonStateChangedObservable()
{
  return onTriggerStateChangedObservable;
}

Observable<ExtendedGamepadButton>& WindowsMotionController::get_onMenuButtonStateChangedObservable()
{
  return onSecondaryButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>& WindowsMotionController::get_onGripButtonStateChangedObservable()
{
  return onMainButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
WindowsMotionController::get_onThumbstickButtonStateChangedObservable()
{
  return onPadStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
WindowsMotionController::get_onTouchpadButtonStateChangedObservable()
{
  return onTrackpadChangedObservable;
}

Observable<StickValues>& WindowsMotionController::get_onTouchpadValuesChangedObservable()
{
  return onTrackpadValuesChangedObservable;
}

void WindowsMotionController::_updateTrackpad()
{
  if (browserGamepad->axes.size() >= 4) {
    if (!stl_util::almost_equal(browserGamepad->axes[2], trackpad.x)
        || !stl_util::almost_equal(browserGamepad->axes[3], trackpad.y)) {
      trackpad.x = browserGamepad->axes[static_cast<size_t>(
        stl_util::index_of(_mappingAxisMeshNames, "TOUCHPAD_TOUCH_X"))];
      trackpad.y = browserGamepad->axes[static_cast<size_t>(
        stl_util::index_of(_mappingAxisMeshNames, "TOUCHPAD_TOUCH_Y"))];
      onTrackpadValuesChangedObservable.notifyObservers(&trackpad);
    }
  }
}

void WindowsMotionController::update()
{
  WebVRController::update();
  if (!browserGamepad->axes.empty()) {
    _updateTrackpad();
    // Only need to animate axes if there is a loaded mesh
    if (_loadedMeshInfo) {
      for (unsigned int axis = 0; axis < _mappingAxisMeshNames.size(); ++axis) {
        if (axis < browserGamepad->axes.size()) {
          _lerpAxisTransform(axis, browserGamepad->axes[axis]);
        }
      }
    }
  }
}

void WindowsMotionController::_handleButtonChange(unsigned int buttonIdx,
                                                  const ExtendedGamepadButton& state,
                                                  const GamepadButtonChanges& /*changes*/)
{
  if (buttonIdx < _mappingButtons.size()) {
    return;
  }

  auto buttonName = _mappingButtons[buttonIdx];
  if (buttonName.empty()) {
    return;
  }

  // Update the trackpad to ensure trackpad.x/y are accurate during button
  // events between frames
  _updateTrackpad();

  auto _state = state;

  // Only emit events for buttons that we know how to map from index to name
  if (stl_util::contains(_mappingButtonObservableNames, buttonName)) {
    auto observableName = _mappingButtonObservableNames[buttonName];
    if (observableName == "onTriggerStateChangedObservable") {
      onTriggerStateChangedObservable.notifyObservers(&_state);
    }
    else if (observableName == "onSecondaryButtonStateChangedObservable") {
      onSecondaryButtonStateChangedObservable.notifyObservers(&_state);
    }
    else if (observableName == "onMainButtonStateChangedObservable") {
      onMainButtonStateChangedObservable.notifyObservers(&_state);
    }
    else if (observableName == "onPadStateChangedObservable") {
      onPadStateChangedObservable.notifyObservers(&_state);
    }
    else if (observableName == "onTrackpadChangedObservable") {
      onTrackpadChangedObservable.notifyObservers(&_state);
    }
  }

  _lerpButtonTransform(buttonName, static_cast<float>(state.value()));
}

void WindowsMotionController::_lerpButtonTransform(const std::string& buttonName, float buttonValue)
{
  // If there is no loaded mesh, there is nothing to transform.
  if (!_loadedMeshInfo) {
    return;
  }

  if (!stl_util::contains((*_loadedMeshInfo).buttonMeshes, buttonName)) {
    return;
  }

  auto meshInfo = (*_loadedMeshInfo).buttonMeshes.at(buttonName);
  if (!meshInfo || !(*meshInfo).unpressed->rotationQuaternion()
      || !(*meshInfo).pressed->rotationQuaternion() || !(*meshInfo).value->rotationQuaternion()) {
    return;
  }

  Quaternion::SlerpToRef(*(*meshInfo).unpressed->rotationQuaternion(), //
                         *(*meshInfo).pressed->rotationQuaternion(),   //
                         buttonValue,                                  //
                         *(*meshInfo).value->rotationQuaternion());
  Vector3::LerpToRef((*meshInfo).unpressed->position(), //
                     (*meshInfo).pressed->position(),   //
                     buttonValue,                       //
                     (*meshInfo).value->position());
}

void WindowsMotionController::_lerpAxisTransform(unsigned int axis, float axisValue)
{
  if (!_loadedMeshInfo) {
    return;
  }

  if (!stl_util::contains((*_loadedMeshInfo).axisMeshes, axis)) {
    return;
  }

  auto meshInfo = (*_loadedMeshInfo).axisMeshes.at(axis);
  if (!meshInfo || !(*meshInfo).min->rotationQuaternion() || !(*meshInfo).max->rotationQuaternion()
      || !(*meshInfo).value->rotationQuaternion()) {
    return;
  }

  // Convert from gamepad value range (-1 to +1) to lerp range (0 to 1)
  auto lerpValue = axisValue * 0.5f + 0.5f;
  Quaternion::SlerpToRef(*(*meshInfo).min->rotationQuaternion(), //
                         *(*meshInfo).max->rotationQuaternion(), //
                         lerpValue,                              //
                         *(*meshInfo).value->rotationQuaternion());
  Vector3::LerpToRef((*meshInfo).min->position(), //
                     (*meshInfo).max->position(), //
                     lerpValue,                   //
                     (*meshInfo).value->position());
}

void WindowsMotionController::initControllerMesh(
  Scene* /*scene*/, const std::function<void(AbstractMesh* mesh)>& /*meshLoaded*/)
{
}

LoadedMeshInfo WindowsMotionController::processModel(Scene* scene,
                                                     const std::vector<AbstractMesh*>& meshes)
{
  LoadedMeshInfo loadedMeshInfo;

  // Create a new mesh to contain the glTF hierarchy
  auto parentMesh = Mesh::New(id + " " + hand, scene);

  // Find the root node in the loaded glTF scene, and attach it as a child of
  // 'parentMesh'
  AbstractMesh* childMesh = nullptr;
  for (auto& iMesh : meshes) {
    if (!iMesh->parent()) {
      // Exclude controller meshes from picking results
      iMesh->isPickable = false;

      // Handle root node, attach to the new parentMesh
      childMesh = iMesh;
      break;
    }
  }

  if (childMesh) {
    childMesh->setParent(parentMesh.get());

    // Create our mesh info. Note that this method will always return non-null.
    loadedMeshInfo = createMeshInfo(parentMesh.get());
  }
  else {
    BABYLON_LOG_WARN("WindowsMotionController", "Could not find root node in model file.")
  }

  return loadedMeshInfo;
}

LoadedMeshInfo WindowsMotionController::createMeshInfo(AbstractMesh* /*rootNode*/)
{
  return LoadedMeshInfo();
}

Ray WindowsMotionController::getForwardRay(float length)
{
  if (!(_loadedMeshInfo && (*_loadedMeshInfo).pointingPoseNode)) {
    return WebVRController::getForwardRay(length);
  }

  auto m      = (*_loadedMeshInfo).pointingPoseNode->getWorldMatrix();
  auto origin = m.getTranslation();

  Vector3 forward(0.f, 0.f, -1.f);
  auto forwardWorld = Vector3::TransformNormal(forward, m);

  auto direction = Vector3::Normalize(forwardWorld);

  return Ray(origin, direction, length);
}

void WindowsMotionController::dispose()
{
  WebVRController::dispose();

  onTrackpadChangedObservable.clear();
  onTrackpadValuesChangedObservable.clear();
}

} // end of namespace BABYLON
