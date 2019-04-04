#include <babylon/cameras/device_orientation_camera.h>

namespace BABYLON {

bool DeviceOrientationCamera::NodeConstructorAdded = false;

void DeviceOrientationCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "DeviceOrientationCamera", [](const std::string& iName, Scene* scene,
                                  const std::optional<json>& /*options*/) {
      return DeviceOrientationCamera::New(iName, Vector3::Zero(), scene);
    });
  DeviceOrientationCamera::NodeConstructorAdded = true;
}

DeviceOrientationCamera::DeviceOrientationCamera(const std::string& iName,
                                                 const Vector3& position,
                                                 Scene* scene)
    : FreeCamera{iName, position, scene}
    , _initialQuaternion{nullptr}
    , _quaternionCache{std::make_unique<Quaternion>()}
{
  inputs->addDeviceOrientation();
}

DeviceOrientationCamera::~DeviceOrientationCamera()
{
}

Type DeviceOrientationCamera::type() const
{
  return Type::DEVICEORIENTATIONCAMERA;
}

const std::string DeviceOrientationCamera::getClassName() const
{
  return "DeviceOrientationCamera";
}

void DeviceOrientationCamera::_checkInputs()
{
  FreeCamera::_checkInputs();
  _quaternionCache->copyFrom(*rotationQuaternion);
  if (_initialQuaternion) {
    _initialQuaternion->multiplyToRef(*rotationQuaternion, *rotationQuaternion);
  }
}

void DeviceOrientationCamera::resetToCurrentRotation(const Vector3& axis)
{
  // can only work if this camera has a rotation quaternion already.
  if (!rotationQuaternion) {
    return;
  }

  if (!_initialQuaternion) {
    _initialQuaternion = std::make_unique<Quaternion>();
  }

  _initialQuaternion->copyFrom(_quaternionCache ? *_quaternionCache :
                                                  *rotationQuaternion);

  using Vec3Float = float Vector3::*;
  const std::vector<Vec3Float> vecAttrs{&Vector3::x, &Vector3::y, &Vector3::z};
  using QuatFloat = float Quaternion::*;
  const std::vector<QuatFloat> quatAttrs{&Quaternion::x, &Quaternion::y,
                                         &Quaternion::z};

  for (unsigned int i = 0; i < vecAttrs.size(); ++i) {
    if (axis.*vecAttrs[i] == 0.f) {
      (*_initialQuaternion).*quatAttrs[i] = 0.f;
    }
    else {
      (*_initialQuaternion).*quatAttrs[i] *= -1.f;
    }
  }

  _initialQuaternion->normalize();

  // force rotation update
  _initialQuaternion->multiplyToRef(*rotationQuaternion, *rotationQuaternion);
}

} // end of namespace BABYLON
