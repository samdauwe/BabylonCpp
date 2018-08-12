#include <babylon/cameras/virtual_joysticks_camera.h>

namespace BABYLON {

bool VirtualJoysticksCamera::NodeConstructorAdded = false;

void VirtualJoysticksCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "VirtualJoysticksCamera", [](const string_t& name, Scene* scene,
                                 const nullable_t<Json::value>& /*options*/) {
      return VirtualJoysticksCamera::New(name, Vector3::Zero(), scene);
    });
  VirtualJoysticksCamera::NodeConstructorAdded = true;
}

VirtualJoysticksCamera::VirtualJoysticksCamera(const string_t& name,
                                               const Vector3& position,
                                               Scene* scene)
    : FreeCamera{name, position, scene}
{
  inputs->addVirtualJoystick();
}

VirtualJoysticksCamera::~VirtualJoysticksCamera()
{
}

const string_t VirtualJoysticksCamera::getClassName() const
{
  return "VirtualJoysticksCamera";
}

} // end of namespace BABYLON
