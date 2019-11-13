#ifndef BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_GAMEPAD_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_GAMEPAD_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/gamepad_camera.h>

namespace BABYLON {

class AnaglyphGamepadCamera;
using AnaglyphGamepadCameraPtr = std::shared_ptr<AnaglyphGamepadCamera>;

/**
 * @brief Camera used to simulate anaglyphic rendering (based on GamepadCamera).
 * @see http://doc.babylonjs.com/features/cameras#anaglyph-cameras
 */
class BABYLON_SHARED_EXPORT AnaglyphGamepadCamera : public GamepadCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static AnaglyphGamepadCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<AnaglyphGamepadCamera>(
      new AnaglyphGamepadCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~AnaglyphGamepadCamera(); // = default

  /**
   * @brief Gets camera class name.
   * @returns AnaglyphGamepadCamera
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new AnaglyphGamepadCamera.
   * @param name defines camera name
   * @param position defines initial position
   * @param interaxialDistance defines distance between each color axis
   * @param scene defines the hosting scene
   */
  AnaglyphGamepadCamera(const std::string& name, const Vector3& position,
                        float interaxialDistance, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class AnaglyphGamepadCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_GAMEPAD_CAMERA_H
