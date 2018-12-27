#include <babylon/cameras/gamepad_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/inputs/free_camera_gamepad_input.h>

namespace BABYLON {

bool GamepadCamera::NodeConstructorAdded = false;

void GamepadCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "GamepadCamera", [](const std::string& iName, Scene* scene,
                        const std::optional<json>& /*options*/) {
      return GamepadCamera::New(iName, Vector3::Zero(), scene);
    });
  GamepadCamera::NodeConstructorAdded = true;
}

GamepadCamera::GamepadCamera::GamepadCamera(const std::string& name,
                                            const Vector3& position,
                                            Scene* scene)
    : UniversalCamera{name, position, scene}
{
}

GamepadCamera::~GamepadCamera()
{
}

const std::string GamepadCamera::getClassName() const
{
  return "GamepadCamera";
}

} // end of namespace BABYLON
