#include <babylon/cameras/virtual_joysticks_camera.h>

namespace BABYLON {

bool VirtualJoysticksCamera::NodeConstructorAdded = false;

void VirtualJoysticksCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor("VirtualJoysticksCamera",
                           [](const std::string& name, Scene* scene,
                              const std::optional<Json::value>& /*options*/) {
                             return VirtualJoysticksCamera::New(
                               name, Vector3::Zero(), scene);
                           });
  VirtualJoysticksCamera::NodeConstructorAdded = true;
}

VirtualJoysticksCamera::VirtualJoysticksCamera(const std::string& name,
                                               const Vector3& position,
                                               Scene* scene)
    : FreeCamera{name, position, scene}
{
  inputs->addVirtualJoystick();
}

VirtualJoysticksCamera::~VirtualJoysticksCamera()
{
}

const std::string VirtualJoysticksCamera::getClassName() const
{
  return "VirtualJoysticksCamera";
}

} // end of namespace BABYLON
