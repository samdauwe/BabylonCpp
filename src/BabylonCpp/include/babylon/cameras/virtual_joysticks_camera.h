#ifndef BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H
#define BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera.h>

namespace BABYLON {

class VirtualJoysticksCamera;
using VirtualJoysticksCameraPtr = std::shared_ptr<VirtualJoysticksCamera>;

/**
 * @brief This represents a free type of camera. It can be useful in First
 * Person Shooter game for instance. It is identical to the Free Camera and
 * simply adds by default a virtual joystick. Virtual Joysticks are on-screen 2D
 * graphics that are used to control the camera or other scene items.
 * @see http://doc.babylonjs.com/features/cameras#virtual-joysticks-camera
 */
class BABYLON_SHARED_EXPORT VirtualJoysticksCamera : public FreeCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static VirtualJoysticksCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<VirtualJoysticksCamera>(
      new VirtualJoysticksCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~VirtualJoysticksCamera() override; // = default

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Intantiates a VirtualJoysticksCamera. It can be useful in First
   * Person Shooter game for instance. It is identical to the Free Camera and
   * simply adds by default a virtual joystick. Virtual Joysticks are on-screen
   * 2D graphics that are used to control the camera or other scene items.
   * @see http://doc.babylonjs.com/features/cameras#virtual-joysticks-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the start position of the camera in the scene
   * @param scene Define the scene the camera belongs to
   */
  VirtualJoysticksCamera(const std::string& name, const Vector3& position, Scene* scene);

private:
  static bool NodeConstructorAdded;

}; // end of class VirtualJoysticksCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VIRTUAL_JOYSTICKS_CAMERA_H
