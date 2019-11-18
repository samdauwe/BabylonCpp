#ifndef BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_UNIVERSAL_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_UNIVERSAL_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/universal_camera.h>

namespace BABYLON {

class StereoscopicUniversalCamera;
using StereoscopicUniversalCameraPtr = std::shared_ptr<StereoscopicUniversalCamera>;

/**
 * @brief Camera used to simulate stereoscopic rendering (based on
 * UniversalCamera).
 * @see http://doc.babylonjs.com/features/cameras
 */
class BABYLON_SHARED_EXPORT StereoscopicUniversalCamera : public UniversalCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static StereoscopicUniversalCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<StereoscopicUniversalCamera>(
      new StereoscopicUniversalCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~StereoscopicUniversalCamera() override; // = default

  /**
   * @brief Gets camera class name.
   * @returns StereoscopicUniversalCamera
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new StereoscopicUniversalCamera.
   * @param name defines camera name
   * @param position defines initial position
   * @param interaxialDistance defines distance between each color axis
   * @param isStereoscopicSideBySide defines is stereoscopic is done side by
   * side or over under
   * @param scene defines the hosting scene
   */
  StereoscopicUniversalCamera(const std::string& name, const Vector3& position,
                              float interaxialDistance, bool isStereoscopicSideBySide,
                              Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class StereoscopicUniversalCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_UNIVERSAL_CAMERA_H
