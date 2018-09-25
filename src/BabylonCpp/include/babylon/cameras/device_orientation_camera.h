#ifndef BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H
#define BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/math/axis.h>

namespace BABYLON {

class DeviceOrientationCamera;
using DeviceOrientationCameraPtr = std::shared_ptr<DeviceOrientationCamera>;

/**
 * @brief This is a camera specifically designed to react to device orientation
 * events such as a modern mobile device being tilted forward or back and left
 * or right.
 */
class BABYLON_SHARED_EXPORT DeviceOrientationCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static DeviceOrientationCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<DeviceOrientationCamera>(
      new DeviceOrientationCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~DeviceOrientationCamera() override;

  virtual IReflect::Type type() const override;

  /**
   * @brief Gets the current instance class name ("DeviceOrientationCamera").
   * This helps avoiding instanceof at run time.
   * @returns the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Checks and applies the current values of the inputs to the camera.
   * Hidden
   */
  void _checkInputs() override;

  /**
   * @brief Reset the camera to its default orientation on the specified axis
   * only.
   * @param axis The axis to reset
   */
  void resetToCurrentRotation(const Vector3& axis = Axis::Y());

protected:
  /**
   * @brief Creates a new device orientation camera.
   * @param name The name of the camera
   * @param position The start position camera
   * @param scene The scene the camera belongs to
   */
  DeviceOrientationCamera(const std::string& name, const Vector3& position,
                          Scene* scene);

private:
  std::unique_ptr<Quaternion> _initialQuaternion;
  std::unique_ptr<Quaternion> _quaternionCache;

  static bool NodeConstructorAdded;

}; // end of class ArcFollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H
