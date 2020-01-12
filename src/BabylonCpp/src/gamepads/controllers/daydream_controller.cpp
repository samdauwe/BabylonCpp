#include <babylon/gamepads/controllers/daydream_controller.h>

#include <babylon/core/logging.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

bool DaydreamControllerFactory::canCreate(const IBrowserGamepadPtr& gamepadInfo) const
{
  return StringTools::startsWith(gamepadInfo->id, DaydreamController::GAMEPAD_ID_PREFIX);
}

WebVRControllerPtr DaydreamControllerFactory::create(const IBrowserGamepadPtr& gamepadInfo) const
{
  return DaydreamController::New(gamepadInfo);
}

DaydreamController::DaydreamController(const IBrowserGamepadPtr& vrGamepad)
    : WebVRController{vrGamepad}
{
  controllerType = PoseEnabledControllerType::DAYDREAM;
}

DaydreamController::~DaydreamController() = default;

void DaydreamController::initControllerMesh(
  Scene* scene, const std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, DaydreamController::MODEL_BASE_URL, DaydreamController::MODEL_FILENAME, scene,
    [this, &meshLoaded](const std::vector<AbstractMeshPtr>& newMeshes,
                        const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                        const std::vector<SkeletonPtr>& /*skeletons*/,
                        const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      _defaultModel = newMeshes[1];
      attachToMesh(_defaultModel);
      if (meshLoaded) {
        meshLoaded(_defaultModel.get());
      }
    });
}

void DaydreamController::_handleButtonChange(unsigned int buttonIdx,
                                             const ExtendedGamepadButton& state,
                                             const GamepadButtonChanges& /*changes*/)
{
  // Daydream controller only has 1 GamepadButton (on the trackpad).
  if (buttonIdx == 0) {
    auto observable = onTriggerStateChangedObservable;
    {
      ExtendedGamepadButton stateCopy{state};
      observable.notifyObservers(&stateCopy);
    }
  }
  else {
    // If the app or home buttons are ever made available
    BABYLON_LOGF_WARN("DaydreamController", "Unrecognized Daydream button index: %d", buttonIdx)
  }
}

} // end of namespace BABYLON
