#ifndef BABYLON_CAMERAS_TOUCH_CAMERA_H
#define BABYLON_CAMERAS_TOUCH_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

/**
 * @brief We're mainly based on the logic defined into the FreeCamera code.
 */
class BABYLON_SHARED_EXPORT TouchCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static TouchCameraPtr New(Ts&&... args)
  {
    auto camera
      = shared_ptr_t<TouchCamera>(new TouchCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~TouchCamera() override;

  const string_t getClassName() const override;
  void _setupInputs() override;

protected:
  TouchCamera(const string_t& name, const Vector3& position, Scene* scene);

  float get_touchAngularSensibility() const;
  void set_touchAngularSensibility(float value);

  float get_touchMoveSensibility() const;
  void set_touchMoveSensibility(float value);

public:
  //-- Begin properties for backward compatibility for inputs
  Property<TouchCamera, float> touchAngularSensibility;
  Property<TouchCamera, float> touchMoveSensibility;
  //-- end properties for backward compatibility for inputs

private:
  static bool NodeConstructorAdded;

}; // end of class TouchCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_TOUCH_CAMERA_H
