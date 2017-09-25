#include <babylon/gamepad/controllers/windows_motion_controller.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/ray.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

const string_t WindowsMotionController::MODEL_BASE_URL
  = "https://controllers.babylonjs.com/microsoft/";
const string_t WindowsMotionController::MODEL_LEFT_FILENAME  = "left.glb";
const string_t WindowsMotionController::MODEL_RIGHT_FILENAME = "right.glb";

const string_t WindowsMotionController::GAMEPAD_ID_PREFIX
  = "Spatial Controller (Spatial Interaction Source) ";
const string_t WindowsMotionController::GAMEPAD_ID_PATTERN
  = "([0-9a-zA-Z]+-[0-9a-zA-Z]+)";

WindowsMotionController::WindowsMotionController(
  const shared_ptr_t<IBrowserGamepad>& vrGamepad)
    : WebVRController{vrGamepad}
    , _loadedMeshInfo{nullptr}
    ,_mappingButtons{"thumbstick", "trigger", "grip", "menu", "trackpad"}
    , _mappingButtonMeshNames{              //
        {"trigger", "SELECT"},              //
        {"menu", "MENU"},                   //
        {"grip", "GRASP"},                  //
        {"thumbstick", "THUMBSTICK_PRESS"}, //
        {"trackpad", "TOUCHPAD_PRESS"}      //
    }                                       //
    ,_mappingButtonObservableNames {                         //
        {"trigger", "onTriggerStateChangedObservable"},      //
        {"menu", "onSecondaryButtonStateChangedObservable"}, //
        {"grip", "onMainButtonStateChangedObservable"},      //
        {"thumbstick", "onPadStateChangedObservable"},       //
        {"trackpad", "onTrackpadChangedObservable"}          //
    }                                                        //
    ,_mappingAxisMeshNames{ //
        "THUMBSTICK_X",     //
        "THUMBSTICK_Y",     //
        "TOUCHPAD_TOUCH_X", //
        "TOUCHPAD_TOUCH_Y"  //
    }                       //
    ,_mappingPointingPoseMeshName{ "POINTING_POSE"}

{
  controllerType = PoseEnabledControllerType::WINDOWS;
}

WindowsMotionController::~WindowsMotionController()
{
}

Observable<ExtendedGamepadButton>&
WindowsMotionController::onTriggerButtonStateChangedObservable()
{
  return onTriggerStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
WindowsMotionController::onMenuButtonStateChangedObservable()
{
  return onSecondaryButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
WindowsMotionController::onGripButtonStateChangedObservable()
{
  return onMainButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
WindowsMotionController::onThumbstickButtonStateChangedObservable()
{
  return onPadStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
WindowsMotionController::onTouchpadButtonStateChangedObservable()
{
  return onTrackpadChangedObservable;
}

void WindowsMotionController::update()
{
  WebVRController::update();

  // Only need to animate axes if there is a loaded mesh
  if (_loadedMeshInfo) {
  }
}

void WindowsMotionController::handleButtonChange(
  unsigned int buttonIdx, const ExtendedGamepadButton& state,
  const GamepadButtonChanges& /*changes*/)
{
  if (buttonIdx < _mappingButtons.size()) {
    return;
  }

  auto buttonName = _mappingButtons[buttonIdx];
  if (buttonName.empty()) {
    return;
  }

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

  lerpButtonTransform(buttonName, state.value());
}

void WindowsMotionController::lerpButtonTransform(const string_t& buttonName,
                                                  float buttonValue)
{
  // If there is no loaded mesh, there is nothing to transform.
  if (!_loadedMeshInfo) {
    return;
  }

  if (!stl_util::contains((*_loadedMeshInfo).buttonMeshes, buttonName)) {
    return;
  }

  auto meshInfo = (*_loadedMeshInfo).buttonMeshes.at(buttonName);
  if (!meshInfo) {
    return;
  }

  Quaternion::SlerpToRef((*meshInfo).unpressed->rotationQuaternion(), //
                         (*meshInfo).pressed->rotationQuaternion(),   //
                         buttonValue,                                 //
                         (*meshInfo).value->rotationQuaternion());
  Vector3::LerpToRef((*meshInfo).unpressed->position(), //
                     (*meshInfo).pressed->position(),   //
                     buttonValue,                       //
                     (*meshInfo).value->position());
}

void WindowsMotionController::lerpAxisTransform(unsigned int axis,
                                                float axisValue)
{
  if (!_loadedMeshInfo) {
    return;
  }

  if (!stl_util::contains((*_loadedMeshInfo).axisMeshes, axis)) {
    return;
  }

  auto meshInfo = (*_loadedMeshInfo).axisMeshes.at(axis);
  if (!meshInfo) {
    return;
  }

  // Convert from gamepad value range (-1 to +1) to lerp range (0 to 1)
  auto lerpValue = axisValue * 0.5f + 0.5f;
  Quaternion::SlerpToRef((*meshInfo).min->rotationQuaternion(), //
                         (*meshInfo).max->rotationQuaternion(), //
                         lerpValue,                             //
                         (*meshInfo).value->rotationQuaternion());
  Vector3::LerpToRef((*meshInfo).min->position(), //
                     (*meshInfo).max->position(), //
                     lerpValue,                   //
                     (*meshInfo).value->position());
}

void WindowsMotionController::initControllerMesh(
  Scene* /*scene*/,
  const ::std::function<void(AbstractMesh* mesh)>& /*meshLoaded*/)
{
}

LoadedMeshInfo
WindowsMotionController::processModel(Scene* /*scene*/,
                                      const vector_t<AbstractMesh*>& /*meshes*/)
{
  return LoadedMeshInfo();
}

LoadedMeshInfo
WindowsMotionController::createMeshInfo(AbstractMesh* /*rootNode*/)
{
  return LoadedMeshInfo();
}

Ray WindowsMotionController::getForwardRay(float length)
{
  if (!(_loadedMeshInfo && (*_loadedMeshInfo).pointingPoseNode)) {
    return WebVRController::getForwardRay(length);
  }

  auto m      = *(*_loadedMeshInfo).pointingPoseNode->getWorldMatrix();
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
}

} // end of namespace BABYLON
