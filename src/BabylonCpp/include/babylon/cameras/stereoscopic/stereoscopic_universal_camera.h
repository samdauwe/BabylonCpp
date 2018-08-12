#ifndef BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_UNIVERSAL_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_UNIVERSAL_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/universal_camera.h>

namespace BABYLON {

/**
 * @brief Camera used to simulate stereoscopic rendering (based on
 * UniversalCamera).
 */
class BABYLON_SHARED_EXPORT StereoscopicUniversalCamera
    : public UniversalCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static StereoscopicUniversalCamera* New(Ts&&... args)
  {
    auto camera = new StereoscopicUniversalCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~StereoscopicUniversalCamera() override;

  /**
   * @brief Gets camera class name.
   * @returns StereoscopicUniversalCamera
   */
  const string_t getClassName() const override;

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
  StereoscopicUniversalCamera(const string_t& name, const Vector3& position,
                              float interaxialDistance,
                              bool isStereoscopicSideBySide, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class StereoscopicUniversalCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_UNIVERSAL_CAMERA_H
