#include <babylon/gamepads/controllers/gear_vr_controller.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

bool GearVRControllerFactory::canCreate(const IBrowserGamepadPtr& gamepadInfo) const
{
  return StringTools::startsWith(gamepadInfo->id, GearVRController::GAMEPAD_ID_PREFIX)
         || StringTools::contains(gamepadInfo->id, "Oculus Go")
         || StringTools::contains(gamepadInfo->id, "Vive Focus");
}

WebVRControllerPtr GearVRControllerFactory::create(const IBrowserGamepadPtr& gamepadInfo) const
{
  return GearVRController::New(gamepadInfo);
}

GearVRController::GearVRController(const IBrowserGamepadPtr& vrGamepad)
    : WebVRController{vrGamepad}
    , _buttonIndexToObservableNameMap{{
        "onPadStateChangedObservable",    // Pad
        "onTriggerStateChangedObservable" // Trigger}
      }}
{
  controllerType = PoseEnabledControllerType::GEAR_VR;
  // Initial starting position defaults to where hand would be (incase of only 3dof controller)
  _calculatedPosition = Vector3(hand == "left" ? -0.15f : 0.15f, -0.5f, 0.25f);
  _disableTrackPosition(_calculatedPosition);
}

GearVRController::~GearVRController() = default;

void GearVRController::initControllerMesh(Scene* scene,
                                          const std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, GearVRController::MODEL_BASE_URL, GearVRController::MODEL_FILENAME, scene,
    [this, &scene, &meshLoaded](const std::vector<AbstractMeshPtr>& newMeshes,
                                const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                                const std::vector<SkeletonPtr>& /*skeletons*/,
                                const std::vector<AnimationGroupPtr>& /*animationGroups*/,
                                const std::vector<TransformNodePtr>& /*transformNodes*/,
                                const std::vector<GeometryPtr>& /*geometries*/,
                                const std::vector<LightPtr>& /*lights*/) {
      // Offset the controller so it will rotate around the users wrist
      auto mesh                  = Mesh::New("", scene);
      newMeshes[1]->parent       = mesh.get();
      newMeshes[1]->position().z = -0.15f;
      _defaultModel              = mesh;
      attachToMesh(_defaultModel);
      if (meshLoaded) {
        meshLoaded(_defaultModel.get());
      }
    });
}

void GearVRController::_handleButtonChange(unsigned int buttonIdx,
                                           const ExtendedGamepadButton& state,
                                           const GamepadButtonChanges& /*changes*/)
{
  if (buttonIdx < _buttonIndexToObservableNameMap.size()) {
    const auto observableName = _buttonIndexToObservableNameMap[buttonIdx];

    // Only emit events for buttons that we know how to map from index to observable
    auto observable = (observableName == "onPadStateChangedObservable") ?
                        onPadStateChangedObservable :
                        onTriggerStateChangedObservable;
    {
      ExtendedGamepadButton stateCopy{state};
      observable.notifyObservers(&stateCopy);
    }
  }
}

} // end of namespace BABYLON
