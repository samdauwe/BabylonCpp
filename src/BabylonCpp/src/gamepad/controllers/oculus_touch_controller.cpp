#include <babylon/gamepad/controllers/oculus_touch_controller.h>

#include <babylon/engine/node.h>
#include <babylon/gamepad/controllers/extended_gamepad_button.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

OculusTouchController::OculusTouchController(
  const std::shared_ptr<IBrowserGamepad>& vrGamepad)
    : WebVRController{vrGamepad}
    , onAButtonStateChangedObservable{this,
                                      &OculusTouchController::
                                        get_onAButtonStateChangedObservable}
    , onBButtonStateChangedObservable{this,
                                      &OculusTouchController::
                                        get_onBButtonStateChangedObservable}
    , onXButtonStateChangedObservable{this,
                                      &OculusTouchController::
                                        get_onXButtonStateChangedObservable}
    , onYButtonStateChangedObservable{
        this, &OculusTouchController::get_onYButtonStateChangedObservable}
{
  controllerType = PoseEnabledControllerType::OCULUS;
  _defaultModel  = nullptr;
}

OculusTouchController::~OculusTouchController()
{
}

void OculusTouchController::initControllerMesh(
  Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  std::string meshName;

  // Hand
  if (hand == "left") {
    meshName = OculusTouchController::MODEL_LEFT_FILENAME;
  }
  else { // Right is the default if no hand is specified
    meshName = OculusTouchController::MODEL_RIGHT_FILENAME;
  }

  SceneLoader::ImportMesh(
    {}, OculusTouchController::MODEL_BASE_URL, meshName, scene,
    [this,
     &meshLoaded](const std::vector<AbstractMeshPtr>& newMeshes,
                  const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                  const std::vector<SkeletonPtr>& /*skeletons*/,
                  const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      /*
      Parent Mesh name: oculus_touch_left
      - body
      - trigger
      - thumbstick
      - grip
      - button_y
      - button_x
      - button_enter
      */
      _defaultModel = newMeshes[1];
      attachToMesh(_defaultModel);
      if (meshLoaded) {
        meshLoaded(_defaultModel.get());
      }
    });
}

Observable<ExtendedGamepadButton>&
OculusTouchController::get_onAButtonStateChangedObservable()
{
  if (hand == "right") {
    return onMainButtonStateChangedObservable;
  }
  else {
    throw ::std::runtime_error("No A button on left hand");
  }
}

Observable<ExtendedGamepadButton>&
OculusTouchController::get_onBButtonStateChangedObservable()
{
  if (hand == "right") {
    return onSecondaryButtonStateChangedObservable;
  }
  else {
    throw ::std::runtime_error("No B button on left hand");
  }
}

Observable<ExtendedGamepadButton>&
OculusTouchController::get_onXButtonStateChangedObservable()
{
  if (hand == "left") {
    return onMainButtonStateChangedObservable;
  }
  else {
    throw ::std::runtime_error("No A button on right hand");
  }
}

Observable<ExtendedGamepadButton>&
OculusTouchController::get_onYButtonStateChangedObservable()
{
  if (hand == "left") {
    return onSecondaryButtonStateChangedObservable;
  }
  else {
    throw ::std::runtime_error("No B button on right hand");
  }
}

void OculusTouchController::_handleButtonChange(
  unsigned int buttonIdx, const ExtendedGamepadButton& state,
  const GamepadButtonChanges& /*changes*/)
{
  auto notifyObject     = state; //{ state: state, changes: changes };
  auto triggerDirection = hand == "right" ? -1 : 1;
  auto defaultModelChildren
    = ::std::static_pointer_cast<Node>(_defaultModel)->getChildren();
  switch (buttonIdx) {
    case 0:
      onPadStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 1: // index trigger
      if (_defaultModel) {
        (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[3]))
          ->rotation()
          .x
          = -notifyObject.value() * 0.20f;
        (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[3]))
          ->position()
          .y
          = -notifyObject.value() * 0.005f;
        (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[3]))
          ->position()
          .z
          = -notifyObject.value() * 0.005f;
      }
      onTriggerStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 2: // secondary trigger
      if (_defaultModel) {
        (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[4]))
          ->position()
          .x
          = triggerDirection * notifyObject.value() * 0.0035f;
      }
      onSecondaryTriggerStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 3:
      if (_defaultModel) {
        if (notifyObject.pressed()) {
          (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[1]))
            ->position()
            .y
            = -0.001f;
        }
        else {
          (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[1]))
            ->position()
            .y
            = 0.f;
        }
      }
      onMainButtonStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 4:
      if (_defaultModel) {
        if (notifyObject.pressed()) {
          (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[2]))
            ->position()
            .y
            = -0.001f;
        }
        else {
          (::std::static_pointer_cast<AbstractMesh>(defaultModelChildren[2]))
            ->position()
            .y
            = 0.f;
        }
      }
      onSecondaryButtonStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 5:
      onThumbRestChangedObservable.notifyObservers(&notifyObject);
      return;
  }
}

} // end of namespace BABYLON
