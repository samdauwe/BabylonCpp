#ifndef BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_UNIVERSAL_CAMERA_H
#define BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_UNIVERSAL_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/universal_camera.h>

namespace BABYLON {

/**
 * @brief Camera used to simulate anaglyphic rendering (based on
 * UniversalCamera).
 */
class BABYLON_SHARED_EXPORT AnaglyphUniversalCamera : public UniversalCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static AnaglyphUniversalCameraPtr New(Ts&&... args)
  {
    auto camera = shared_ptr_t<AnaglyphUniversalCamera>(
      new AnaglyphUniversalCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~AnaglyphUniversalCamera() override;

  /**
   * @brief Gets camera class name.
   * @returns AnaglyphUniversalCamera
   */
  const string_t getClassName() const override;

protected:
  /**
   * @brief Creates a new AnaglyphUniversalCamera.
   * @param name defines camera name
   * @param position defines initial position
   * @param interaxialDistance defines distance between each color axis
   * @param scene defines the hosting scene
   */
  AnaglyphUniversalCamera(const string_t& name, const Vector3& position,
                          float interaxialDistance, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class AnaglyphUniversalCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_STEREOSCOPIC_ANAGLYPH_UNIVERSAL_CAMERA_H
