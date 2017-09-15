#ifndef BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H
#define BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/math/axis.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT DeviceOrientationCamera : public FreeCamera {

public:
  template <typename... Ts>
  static DeviceOrientationCamera* New(Ts&&... args)
  {
    auto camera = new DeviceOrientationCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~DeviceOrientationCamera();

  virtual IReflect::Type type() const override;

  const char* getClassName() const override;
  void _checkInputs() override;
  void resetToCurrentRotation(const Vector3& axis = Axis::Y);

protected:
  DeviceOrientationCamera(const string_t& name, const Vector3& position,
                          Scene* scene);

private:
  unique_ptr_t<Quaternion> _initialQuaternion;
  unique_ptr_t<Quaternion> _quaternionCache;

}; // end of class ArcFollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H
