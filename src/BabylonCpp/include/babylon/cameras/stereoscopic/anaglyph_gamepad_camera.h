#ifndef BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_GAMEPAD_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_GAMEPAD_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/gamepad_camera.h>

namespace BABYLON {

/**
 * @brief Camera used to simulate anaglyphic rendering (based on GamepadCamera).
 */
class BABYLON_SHARED_EXPORT AnaglyphGamepadCamera : public GamepadCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static AnaglyphGamepadCamera* New(Ts&&... args)
  {
    auto camera = new AnaglyphGamepadCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~AnaglyphGamepadCamera() override;

  /**
   * @brief Gets camera class name.
   * @returns AnaglyphGamepadCamera
   */
  const string_t getClassName() const override;

protected:
  /**
   * @brief Creates a new AnaglyphGamepadCamera.
   * @param name defines camera name
   * @param position defines initial position
   * @param interaxialDistance defines distance between each color axis
   * @param scene defines the hosting scene
   */
  AnaglyphGamepadCamera(const string_t& name, const Vector3& position,
                        float interaxialDistance, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class AnaglyphGamepadCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_GAMEPAD_CAMERA_H
