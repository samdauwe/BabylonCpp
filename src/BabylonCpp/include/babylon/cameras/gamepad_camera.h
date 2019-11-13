#ifndef BABYLON_CAMERAS_GAMEPAD_CAMERA_H
#define BABYLON_CAMERAS_GAMEPAD_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/universal_camera.h>

namespace BABYLON {

class GamepadCamera;
using GamepadCameraPtr = std::shared_ptr<GamepadCamera>;

/**
 * @brief This represents a FPS type of camera. This is only here for back
 * compat purpose. Please use the UniversalCamera instead as both are identical.
 * @see http://doc.babylonjs.com/features/cameras#universal-camera
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
  ~GamepadCamera(); // = default

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Instantiates a new Gamepad Camera.
   * This represents a FPS type of camera. This is only here for back compat
   * purpose. Please use the UniversalCamera instead as both are identical.
   * @see http://doc.babylonjs.com/features/cameras#universal-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the start position of the camera in the scene
   * @param scene Define the scene the camera belongs to
   */
  GamepadCamera(const std::string& name, const Vector3& position, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class GamepadCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_GAMEPAD_CAMERA_H
