#include <babylon/gamepad/controllers/generic_controller.h>

#include <babylon/core/logging.h>
#include <babylon/gamepad/controllers/extended_gamepad_button.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {

GenericController::GenericController(
  const shared_ptr_t<IBrowserGamepad>& vrGamepad)
    : WebVRController{vrGamepad}
{
}

GenericController::~GenericController()
{
}

void GenericController::initControllerMesh(
  Scene* scene, const ::std::function<void(AbstractMesh* mesh)>& meshLoaded)
{
  SceneLoader::ImportMesh(
    {}, "http://yoda.blob.core.windows.net/models/",
    "genericvrcontroller.babylon", scene,
    [this, &meshLoaded](vector_t<AbstractMesh*>& newMeshes,
                        vector_t<ParticleSystem*>& /*particleSystems*/,
                        vector_t<Skeleton*>& /*skeletons*/) {
      _defaultModel = newMeshes[1];
      if (meshLoaded) {
        meshLoaded(_defaultModel);
      }
      attachToMesh(_defaultModel);
    });
}

void GenericController::handleButtonChange(unsigned int buttonIdx,
                                           const ExtendedGamepadButton& state,
                                           const GamepadButtonChanges& changes)
{
  BABYLON_LOGF_INFO("GenericController", "Button id: %d", buttonIdx);
  BABYLON_LOGF_INFO("GenericController", "State: %s", state.toString().c_str());
  BABYLON_LOGF_INFO("GenericController", "Changes: %s",
                    changes.toString().c_str());
}

} // end of namespace BABYLON
