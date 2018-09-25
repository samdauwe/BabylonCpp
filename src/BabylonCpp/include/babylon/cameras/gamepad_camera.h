#ifndef BABYLON_CAMERAS_GAMEPAD_CAMERA_H
#define BABYLON_CAMERAS_GAMEPAD_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/universal_camera.h>

namespace BABYLON {

class GamepadCamera;
using GamepadCameraPtr = std::shared_ptr<GamepadCamera>;

/**
 * @brief We're mainly based on the logic defined into the FreeCamera code.
 */
class BABYLON_SHARED_EXPORT GamepadCamera : public UniversalCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static GamepadCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<GamepadCamera>(
      new GamepadCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~GamepadCamera() override;

  const std::string getClassName() const override;

protected:
  GamepadCamera(const std::string& name, const Vector3& position, Scene* scene);

  float get_gamepadAngularSensibility() const;
  void set_gamepadAngularSensibility(float value);

  float get_gamepadMoveSensibility() const;
  void set_gamepadMoveSensibility(float value);

public:
  //-- Begin properties for backward compatibility for inputs
  Property<GamepadCamera, float> gamepadAngularSensibility;
  Property<GamepadCamera, float> gamepadMoveSensibility;
  //-- end properties for backward compatibility for inputs

private:
  static bool NodeConstructorAdded;

}; // end of class GamepadCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_GAMEPAD_CAMERA_H
