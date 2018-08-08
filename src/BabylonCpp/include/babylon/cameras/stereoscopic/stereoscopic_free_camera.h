#ifndef BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_FREE_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_FREE_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

/**
 * @brief Camera used to simulate stereoscopic rendering (based on FreeCamera).
 */
class BABYLON_SHARED_EXPORT StereoscopicFreeCamera : public FreeCamera {

public:
  template <typename... Ts>
  static StereoscopicFreeCamera* New(Ts&&... args)
  {
    auto camera = new StereoscopicFreeCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));
    if (!StereoscopicFreeCamera::NodeConstructorAdded
        && StereoscopicFreeCamera::AddNodeConstructor) {
      StereoscopicFreeCamera::AddNodeConstructor();
    }

    return camera;
  }
  ~StereoscopicFreeCamera() override;

  /**
   * @brief Gets camera class name.
   * @returns StereoscopicFreeCamera
   */
  const string_t getClassName() const override;

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
  StereoscopicFreeCamera(const string_t& name, const Vector3& position,
                         float interaxialDistance,
                         bool isStereoscopicSideBySide, Scene* scene);

private:
  static bool NodeConstructorAdded;
  static ::std::function<void()> AddNodeConstructor;

}; // end of class StereoscopicFreeCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_FREE_CAMERA_H
