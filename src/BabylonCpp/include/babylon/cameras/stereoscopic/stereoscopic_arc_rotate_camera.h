#ifndef BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_ARC_ROTATE_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_ARC_ROTATE_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/arc_rotate_camera.h>

namespace BABYLON {

/**
 * @brief Camera used to simulate stereoscopic rendering (based on
 * ArcRotateCamera).
 */
class BABYLON_SHARED_EXPORT StereoscopicArcRotateCamera
    : public ArcRotateCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static StereoscopicArcRotateCamera* New(Ts&&... args)
  {
    auto camera = new StereoscopicArcRotateCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~StereoscopicArcRotateCamera() override;

  /**
   * @brief Gets camera class name
   * @returns StereoscopicArcRotateCamera
   */
  const string_t getClassName() const override;

protected:
  /**
   * @brief Creates a new StereoscopicArcRotateCamera
   * @param name defines camera name
   * @param alpha defines alpha angle (in radians)
   * @param beta defines beta angle (in radians)
   * @param radius defines radius
   * @param target defines camera target
   * @param interaxialDistance defines distance between each color axis
   * @param isStereoscopicSideBySide defines is stereoscopic is done side by
   * side or over under
   * @param scene defines the hosting scene
   */
  StereoscopicArcRotateCamera(const string_t& name, float alpha, float beta,
                              float radius, const Vector3& target,
                              float interaxialDistance,
                              bool isStereoscopicSideBySide, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class StereoscopicArcRotateCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_STEREOSCOPIC_ARC_ROTATE_CAMERA_H
