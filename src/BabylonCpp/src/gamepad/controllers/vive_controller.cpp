#include <babylon/gamepad/controllers/vive_controller.h>

#include <babylon/gamepad/controllers/extended_gamepad_button.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

ViveController::ViveController(const shared_ptr_t<IBrowserGamepad>& vrGamepad)
    : WebVRController{vrGamepad}
{
  _defaultModel     = nullptr;
  controllerType    = PoseEnabledControllerType::VIVE;
  _invertLeftStickY = true;
}

ViveController::~ViveController()
{
}

void ViveController::initControllerMesh(
  Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, ViveController::MODEL_BASE_URL, ViveController::MODEL_FILENAME, scene,
    [this, &meshLoaded](vector_t<AbstractMesh*>& newMeshes,
                        vector_t<ParticleSystem*>& /*particleSystems*/,
                        vector_t<Skeleton*>& /*skeletons*/) {
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
        meshLoaded(_defaultModel);
      }
    });
}

Observable<ExtendedGamepadButton>&
ViveController::onLeftButtonStateChangedObservable()
{
  return onMainButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
ViveController::onRightButtonStateChangedObservable()
{
  return onMainButtonStateChangedObservable;
}

Observable<ExtendedGamepadButton>&
ViveController::onMenuButtonStateChangedObservable()
{
  return onSecondaryButtonStateChangedObservable;
}

void ViveController::_handleButtonChange(
  unsigned int buttonIdx, const ExtendedGamepadButton& state,
  const GamepadButtonChanges& /*changes*/)
{
  auto notifyObject         = state; //{ state: state, changes: changes };
  auto defaultModelChildren = dynamic_cast<Node*>(_defaultModel)->getChildren();
  switch (buttonIdx) {
    case 0:
      onPadStateChangedObservable.notifyObservers(&notifyObject);
      return;
    case 1: // index trigger
      if (_defaultModel) {
        (static_cast<AbstractMesh*>(defaultModelChildren[6]))->rotation().x
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
          (static_cast<AbstractMesh*>(defaultModelChildren[2]))->position().y
            = -0.001f;
        }
        else {
          (static_cast<AbstractMesh*>(defaultModelChildren[2]))->position().y
            = 0.f;
        }
      }
      onSecondaryButtonStateChangedObservable.notifyObservers(&notifyObject);
      return;
  }
}

} // end of namespace BABYLON
