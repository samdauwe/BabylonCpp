#include <babylon/gamepads/controllers/vive_controller.h>

#include <babylon/core/string.h>
#include <babylon/gamepads/controllers/extended_gamepad_button.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

bool ViveControllerFactory::canCreate(
  const IBrowserGamepadPtr& gamepadInfo) const
{
  return String::contains(String::toLowerCase(gamepadInfo->id), "openvr");
}

WebVRControllerPtr
ViveControllerFactory::create(const IBrowserGamepadPtr& gamepadInfo) const
{
  return ViveController::New(gamepadInfo);
}

ViveController::ViveController(const IBrowserGamepadPtr& vrGamepad)
    : WebVRController{vrGamepad}
    , onLeftButtonStateChangedObservable{this,
                                         &ViveController::
                                           get_onLeftButtonStateChangedObservable}
    , onRightButtonStateChangedObservable{this,
                                          &ViveController::
                                            get_onRightButtonStateChangedObservable}
    , onMenuButtonStateChangedObservable{
        this, &ViveController::get_onMenuButtonStateChangedObservable}
{
  _defaultModel     = nullptr;
  controllerType    = PoseEnabledControllerType::VIVE;
  _invertLeftStickY = true;
}

ViveController::~ViveController() = default;

void ViveController::initControllerMesh(
  Scene* scene, const std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, ViveController::MODEL_BASE_URL, ViveController::MODEL_FILENAME, scene,
    [this,
     &meshLoaded](const std::vector<AbstractMeshPtr>& newMeshes,
                  const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                  const std::vector<SkeletonPtr>& /*skeletons*/,
                  const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      /*
      Parent Mesh name: ViveWand
      - body
      - r_gripper
      - l_gripper
      - menu_button
      - system_button
      - trackpad
      - trigger
      - LED
      */
      _defaultModel = newMeshes[1];
      attachToMesh(_defaultModel);
      if (meshLoaded) {
        meshLoaded(_defaultModel.get());
      }
    });
}

Observable<ExtendedGamepadButton>&
ViveController::get_onLeftButtonStateChangedObservable()
{
  return onMainButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
ViveController::get_onRightButtonStateChangedObservable()
{
  return onMainButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
ViveController::get_onMenuButtonStateChangedObservable()
{
  return onSecondaryButtonStateChangedObservable;
}

void ViveController::_handleButtonChange(
  unsigned int buttonIdx, const ExtendedGamepadButton& state,
  const GamepadButtonChanges& /*changes*/)
{
  auto notifyObject = state; //{ state: state, changes: changes };
  auto defaultModelChildren
    = std::static_pointer_cast<Node>(_defaultModel)->getChildren();
  switch (buttonIdx) {
    case 0:
      onPadStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 1: // index trigger
      if (_defaultModel) {
        (std::static_pointer_cast<AbstractMesh>(defaultModelChildren[6]))
          ->rotation()
          .x
          = -notifyObject.value() * 0.15f;
      }
      onTriggerStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 2: // left AND right button
      onMainButtonStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 3:
      if (_defaultModel) {
        if (notifyObject.pressed()) {
          (std::static_pointer_cast<AbstractMesh>(defaultModelChildren[2]))
            ->position()
            .y
            = -0.001f;
        }
        else {
          (std::static_pointer_cast<AbstractMesh>(defaultModelChildren[2]))
            ->position()
            .y
            = 0.f;
        }
      }
      onSecondaryButtonStateChangedObservable.notifyObservers(&notifyObject);
      return;
  }
}

} // end of namespace BABYLON
