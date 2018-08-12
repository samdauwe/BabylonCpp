#ifndef BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H
#define BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

/**
 * @brief We're mainly based on the logic defined into the FreeCamera code.
 */
class BABYLON_SHARED_EXPORT VirtualJoysticksCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static VirtualJoysticksCamera* New(Ts&&... args)
  {
    auto camera = new VirtualJoysticksCamera(::std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~VirtualJoysticksCamera() override;

  const string_t getClassName() const override;

protected:
  VirtualJoysticksCamera(const string_t& name, const Vector3& position,
                         Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class VirtualJoysticksCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H
