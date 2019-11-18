#ifndef BABYLON_CAMERAS_UNIVERSAL_CAMERA_H
#define BABYLON_CAMERAS_UNIVERSAL_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/touch_camera.h>

namespace BABYLON {

class UniversalCamera;
using UniversalCameraPtr = std::shared_ptr<UniversalCamera>;

/**
 * @brief The Universal Camera is the one to choose for first person shooter
 * type games, and works with all the keyboard, mouse, touch and gamepads. This
 * replaces the earlier Free Camera, which still works and will still be found
 * in many Playgrounds.
 * @see http://doc.babylonjs.com/features/cameras#universal-camera
 */
class BABYLON_SHARED_EXPORT UniversalCamera : public TouchCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static UniversalCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<UniversalCamera>(new UniversalCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~UniversalCamera() override; // = default

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief The Universal Camera is the one to choose for first person shooter
   * type games, and works with all the keyboard, mouse, touch and gamepads.
   * This replaces the earlier Free Camera, which still works and will still be
   * found in many Playgrounds.
   * @see http://doc.babylonjs.com/features/cameras#universal-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the start position of the camera in the scene
   * @param scene Define the scene the camera belongs to
   */
  UniversalCamera(const std::string& name, const Vector3& position, Scene* scene);

  float get_gamepadAngularSensibility() const;
  void set_gamepadAngularSensibility(float value);

  float get_gamepadMoveSensibility() const;
  void set_gamepadMoveSensibility(float value);

public:
  /**
   * Defines the gamepad rotation sensiblity.
   * This is the threshold from when rotation starts to be accounted for to
   * prevent jittering.
   */
  Property<UniversalCamera, float> gamepadAngularSensibility;

  /**
   * Defines the gamepad move sensiblity.
   * This is the threshold from when moving starts to be accounted for for to
   * prevent jittering.
   */
  Property<UniversalCamera, float> gamepadMoveSensibility;

private:
  /** Hidden */
  static bool NodeConstructorAdded;

}; // end of class UniversalCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_UNIVERSAL_CAMERA_H
