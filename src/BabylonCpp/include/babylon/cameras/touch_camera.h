#ifndef BABYLON_CAMERAS_TOUCH_CAMERA_H
#define BABYLON_CAMERAS_TOUCH_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

class TouchCamera;
using TouchCameraPtr = std::shared_ptr<TouchCamera>;

/**
 * @brief This represents a FPS type of camera controlled by touch.
 * This is like a universal camera minus the Gamepad controls.
 * @see https://doc.babylonjs.com/features/cameras#universal-camera
 */
class BABYLON_SHARED_EXPORT TouchCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static TouchCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<TouchCamera>(new TouchCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~TouchCamera() override; // = default

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Hidden
   */
  void _setupInputs() override;

protected:
  /**
   * @brief Instantiates a new touch camera.
   * This represents a FPS type of camera controlled by touch.
   * This is like a universal camera minus the Gamepad controls.
   * @see https://doc.babylonjs.com/features/cameras#universal-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the start position of the camera in the scene
   * @param scene Define the scene the camera belongs to
   */
  TouchCamera(const std::string& name, const Vector3& position, Scene* scene);

  float get_touchAngularSensibility() const;
  void set_touchAngularSensibility(float value);

  float get_touchMoveSensibility() const;
  void set_touchMoveSensibility(float value);

public:
  /**
   * Defines the touch sensibility for rotation.
   * The higher the faster.
   */
  Property<TouchCamera, float> touchAngularSensibility;

  /**
   * Defines the touch sensibility for move.
   * The higher the faster.
   */
  Property<TouchCamera, float> touchMoveSensibility;

private:
  static bool NodeConstructorAdded;

}; // end of class TouchCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_TOUCH_CAMERA_H
