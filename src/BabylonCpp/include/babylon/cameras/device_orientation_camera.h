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
  ~DeviceOrientationCamera(); // = default

  virtual Type type() const override;

  /**
   * @brief Enabled turning on the y axis when the orientation sensor is active.
   * @param dragFactor the factor that controls the turn speed (default: 1/300)
   */
  void enableHorizontalDragging(float dragFactor = 1.f / 300.f);

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

  /**
   * @brief Gets a boolean indicating that pointer input must be disabled on
   * first orientation sensor update (Default: true).
   */
  bool get_disablePointerInputWhenUsingDeviceOrientation() const;

  /**
   * @brief Sets a boolean indicating that pointer input must be disabled on
   * first orientation sensor update (Default: true).
   */
  void set_disablePointerInputWhenUsingDeviceOrientation(bool value);

public:
  /**
   * Gets or sets a boolean indicating that pointer input must be disabled on
   * first orientation sensor update (Default: true)
   */
  Property<DeviceOrientationCamera, bool>
    disablePointerInputWhenUsingDeviceOrientation;

private:
  std::unique_ptr<Quaternion> _initialQuaternion;
  std::unique_ptr<Quaternion> _quaternionCache;
  std::unique_ptr<Quaternion> _tmpDragQuaternion;
  bool _disablePointerInputWhenUsingDeviceOrientation;
  float _dragFactor;

  static bool NodeConstructorAdded;

}; // end of class ArcFollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_DEVICE_ORIENTATION_CAMERA_H
