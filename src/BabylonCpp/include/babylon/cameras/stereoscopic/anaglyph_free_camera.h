#ifndef BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_FREE_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_FREE_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

class AnaglyphFreeCamera;
using AnaglyphFreeCameraPtr = std::shared_ptr<AnaglyphFreeCamera>;

/**
 * @brief Camera used to simulate anaglyphic rendering (based on FreeCamera).
 * @see https://doc.babylonjs.com/features/cameras#anaglyph-cameras
 */
class BABYLON_SHARED_EXPORT AnaglyphFreeCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static AnaglyphFreeCameraPtr New(Ts&&... args)
  {
    auto camera
      = std::shared_ptr<AnaglyphFreeCamera>(new AnaglyphFreeCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~AnaglyphFreeCamera() override; // = default

  /**
   * @brief Gets camera class name
   * @returns AnaglyphFreeCamera
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new AnaglyphFreeCamera.
   * @param name defines camera name
   * @param position defines initial position
   * @param interaxialDistance defines distance between each color axis
   * @param scene defines the hosting scene
   */
  AnaglyphFreeCamera(const std::string& name, const Vector3& position, float interaxialDistance,
                     Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class AnaglyphFreeCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_FREE_CAMERA_H
