#include <babylon/gamepads/controllers/generic_controller.h>

#include <babylon/core/logging.h>
#include <babylon/gamepads/controllers/extended_gamepad_button.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {

bool GenericControllerFactory::canCreate(const IBrowserGamepadPtr& /*gamepadInfo*/) const
{
  return true;
}

WebVRControllerPtr GenericControllerFactory::create(const IBrowserGamepadPtr& gamepadInfo) const
{
  return GenericController::New(gamepadInfo);
}

GenericController::GenericController(const IBrowserGamepadPtr& vrGamepad)
    : WebVRController{vrGamepad}
{
  _defaultModel = nullptr;
}

GenericController::~GenericController() = default;

void GenericController::initControllerMesh(
  Scene* scene, const std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, GenericController::MODEL_BASE_URL, GenericController::MODEL_FILENAME, scene,
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

void GenericController::_handleButtonChange(unsigned int buttonIdx,
                                            const ExtendedGamepadButton& state,
                                            const GamepadButtonChanges& changes)
{
  BABYLON_LOGF_INFO("GenericController", "Button id: %d", buttonIdx)
  BABYLON_LOGF_INFO("GenericController", "State: %s", state.toString().c_str())
  BABYLON_LOGF_INFO("GenericController", "Changes: %s", changes.toString().c_str())
}

} // end of namespace BABYLON
