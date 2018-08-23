#ifndef BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_GAMEPAD_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_GAMEPAD_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/gamepad_camera.h>

namespace BABYLON {

/**
 * @brief Camera used to simulate stereoscopic rendering (based on
 * GamepadCamera).
 */
class BABYLON_SHARED_EXPORT StereoscopicGamepadCamera : public GamepadCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static StereoscopicGamepadCameraPtr New(Ts&&... args)
  {
    auto camera = shared_ptr_t<StereoscopicGamepadCamera>(
      new StereoscopicGamepadCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~StereoscopicGamepadCamera() override;

  /**
   * @brief Gets camera class name.
   * @returns StereoscopicGamepadCamera
   */
  const string_t getClassName() const override;

protected:
  /**
   * @brief Creates a new StereoscopicGamepadCamera.
   * @param name defines camera name
   * @param position defines initial position
   * @param interaxialDistance defines distance between each color axis
   * @param isStereoscopicSideBySide defines is stereoscopic is done side by
   * side or over under
   * @param scene defines the hosting scene
   */
  StereoscopicGamepadCamera(const string_t& name, const Vector3& position,
                            float interaxialDistance,
                            bool isStereoscopicSideBySide, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class StereoscopicGamepadCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_GAMEPAD_CAMERA_H
