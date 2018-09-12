#ifndef BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H
#define BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

class VirtualJoysticksCamera;
using VirtualJoysticksCameraPtr = std::shared_ptr<VirtualJoysticksCamera>;

/**
 * @brief We're mainly based on the logic defined into the FreeCamera code.
 */
class BABYLON_SHARED_EXPORT VirtualJoysticksCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static VirtualJoysticksCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<VirtualJoysticksCamera>(
      new VirtualJoysticksCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~VirtualJoysticksCamera() override;

  const std::string getClassName() const override;

protected:
  VirtualJoysticksCamera(const std::string& name, const Vector3& position,
                         Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class VirtualJoysticksCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H
