#ifndef BABYLON_CAMERAS_GAMEPAD_CAMERA_H
#define BABYLON_CAMERAS_GAMEPAD_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/universal_camera.h>

namespace BABYLON {

/**
 * @brief We're mainly based on the logic defined into the FreeCamera code.
 */
class BABYLON_SHARED_EXPORT GamepadCamera : public UniversalCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static GamepadCamera* New(Ts&&... args)
  {
    auto camera = new GamepadCamera(::std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~GamepadCamera() override;

  const string_t getClassName() const override;

protected:
  GamepadCamera(const string_t& name, const Vector3& position, Scene* scene);

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
