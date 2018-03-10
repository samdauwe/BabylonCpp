#include <babylon/gamepad/controllers/gear_vr_controller.h>

#include <babylon/loading/scene_loader.h>

namespace BABYLON {

GearVRController::GearVRController(
  const shared_ptr_t<IBrowserGamepad>& vrGamepad)
    : WebVRController{vrGamepad}
    , _buttonIndexToObservableNameMap{{
        "onTrackpadChangedObservable",    // Trackpad
        "onTriggerStateChangedObservable" // Trigger}
      }}
{
  controllerType = PoseEnabledControllerType::GEAR_VR;
}

GearVRController::~GearVRController()
{
}

void GearVRController::initControllerMesh(
  Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, GearVRController::MODEL_BASE_URL, GearVRController::MODEL_FILENAME,
    scene,
    [this, &meshLoaded](vector_t<AbstractMesh*>& newMeshes,
                        vector_t<ParticleSystem*>& /*particleSystems*/,
                        vector_t<Skeleton*>& /*skeletons*/) {
      _defaultModel = newMeshes[1];
      attachToMesh(_defaultModel);
      if (meshLoaded) {
        meshLoaded(_defaultModel);
      }
    });
}

void GearVRController::handleButtonChange(
  unsigned int buttonIdx, const ExtendedGamepadButton& state,
  const GamepadButtonChanges& /*changes*/)
{
  if (buttonIdx < _buttonIndexToObservableNameMap.size()) {
    const auto observableName = _buttonIndexToObservableNameMap[buttonIdx];

    // Only emit events for buttons that we know how to map from index to
    // observable
    auto observable = (observableName == "onTrackpadChangedObservable") ?
                        onTrackpadChangedObservable :
                        onTriggerStateChangedObservable;
    {
      ExtendedGamepadButton stateCopy{state};
      observable.notifyObservers(&stateCopy);
    }
  }
}

} // end of namespace BABYLON
