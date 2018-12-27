#include <babylon/cameras/virtual_joysticks_camera.h>

namespace BABYLON {

bool VirtualJoysticksCamera::NodeConstructorAdded = false;

void VirtualJoysticksCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "VirtualJoysticksCamera", [](const std::string& iName, Scene* scene,
                                 const std::optional<json>& /*options*/) {
      return VirtualJoysticksCamera::New(iName, Vector3::Zero(), scene);
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
