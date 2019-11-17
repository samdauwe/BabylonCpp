#ifndef BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_FREE_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_FREE_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

class StereoscopicFreeCamera;
using StereoscopicFreeCameraPtr = std::shared_ptr<StereoscopicFreeCamera>;

/**
 * @brief Camera used to simulate stereoscopic rendering (based on FreeCamera).
 * @see http://doc.babylonjs.com/features/cameras
 */
class BABYLON_SHARED_EXPORT StereoscopicFreeCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static StereoscopicFreeCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<StereoscopicFreeCamera>(
      new StereoscopicFreeCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~StereoscopicFreeCamera() override; // = default

  /**
   * @brief Gets camera class name.
   * @returns StereoscopicFreeCamera
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Creates a new StereoscopicFreeCamera.
   * @param name defines camera name
   * @param position defines initial position
   * @param interaxialDistance defines distance between each color axis
   * @param isStereoscopicSideBySide defines is stereoscopic is done side by
   * side or over under
   * @param scene defines the hosting scene
   */
  StereoscopicFreeCamera(const std::string& name, const Vector3& position, float interaxialDistance,
                         bool isStereoscopicSideBySide, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class StereoscopicFreeCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_FREE_CAMERA_H
