#include <babylon/cameras/device_orientation_camera.h>

namespace BABYLON {

DeviceOrientationCamera::DeviceOrientationCamera(const string_t& iName,
                                                 const Vector3& position,
                                                 Scene* scene)
    : FreeCamera{iName, position, scene}
    , _initialQuaternion{nullptr}
    , _quaternionCache{::std::make_unique<Quaternion>()}
{
  inputs->addDeviceOrientation();
}

DeviceOrientationCamera::~DeviceOrientationCamera()
{
}

IReflect::Type DeviceOrientationCamera::type() const
{
  return IReflect::Type::DEVICEORIENTATIONCAMERA;
}

const char* DeviceOrientationCamera::getClassName() const
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
    _initialQuaternion = ::std::make_unique<Quaternion>();
  }

  _initialQuaternion->copyFrom(_quaternionCache ? *_quaternionCache :
                                                  *rotationQuaternion);

  // x
  if (axis.x == 0.f) {
    _initialQuaternion->x = 0.f;
  }
  else {
    _initialQuaternion->x = -1.f;
  }

  // y
  if (axis.y == 0.f) {
    _initialQuaternion->y = 0.f;
  }
  else {
    _initialQuaternion->y = -1.f;
  }

  // z
  if (axis.z == 0.f) {
    _initialQuaternion->z = 0.f;
  }
  else {
    _initialQuaternion->z = -1.f;
  }

  _initialQuaternion->normalize();

  // force rotation update
  _initialQuaternion->multiplyToRef(*rotationQuaternion, *rotationQuaternion);
}

} // end of namespace BABYLON
