#ifndef BABYLON_CAMERAS_UNIVERSAL_CAMERA_H
#define BABYLON_CAMERAS_UNIVERSAL_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/touch_camera.h>

namespace BABYLON {

class UniversalCamera;
using UniversalCameraPtr = std::shared_ptr<UniversalCamera>;

/**
 * @brief We're mainly based on the logic defined into the FreeCamera code.
 */
class BABYLON_SHARED_EXPORT UniversalCamera : public TouchCamera {

public:
  template <typename... Ts>
  static UniversalCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<UniversalCamera>(
      new UniversalCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~UniversalCamera() override;

  const std::string getClassName() const override;

protected:
  UniversalCamera(const std::string& name, const Vector3& position,
                  Scene* scene);

  float get_gamepadAngularSensibility() const;
  void set_gamepadAngularSensibility(float value);

  float get_gamepadMoveSensibility() const;
  void set_gamepadMoveSensibility(float value);

public:
  //-- Begin properties for backward compatibility for inputs
  Property<UniversalCamera, float> gamepadAngularSensibility;
  Property<UniversalCamera, float> gamepadMoveSensibility;
  //-- end properties for backward compatibility for inputs

}; // end of class UniversalCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_UNIVERSAL_CAMERA_H
