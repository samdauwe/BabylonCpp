#ifndef BABYLON_CAMERAS_UNIVERSAL_CAMERA_H
#define BABYLON_CAMERAS_UNIVERSAL_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/touch_camera.h>

namespace BABYLON {

/**
 * @brief We're mainly based on the logic defined into the FreeCamera code.
 */
class BABYLON_SHARED_EXPORT UniversalCamera : public TouchCamera {

public:
  template <typename... Ts>
  static UniversalCamera* New(Ts&&... args)
  {
    auto camera = new UniversalCamera(::std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~UniversalCamera() override;

  const string_t getClassName() const override;

protected:
  UniversalCamera(const string_t& name, const Vector3& position, Scene* scene);

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
