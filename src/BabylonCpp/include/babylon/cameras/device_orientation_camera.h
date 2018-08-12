#ifndef BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H
#define BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/math/axis.h>

namespace BABYLON {

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
  static DeviceOrientationCamera* New(Ts&&... args)
  {
    auto camera = new DeviceOrientationCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~DeviceOrientationCamera() override;

  virtual IReflect::Type type() const override;

  /**
   * @brief Gets the current instance class name ("DeviceOrientationCamera").
   * This helps avoiding instanceof at run time.
   * @returns the class name
   */
  const string_t getClassName() const override;

  /**
   * @brief Checks and applies the current values of the inputs to the camera.
   * (Internal use only)
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
  DeviceOrientationCamera(const string_t& name, const Vector3& position,
                          Scene* scene);

private:
  unique_ptr_t<Quaternion> _initialQuaternion;
  unique_ptr_t<Quaternion> _quaternionCache;

  static bool NodeConstructorAdded;

}; // end of class ArcFollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H
