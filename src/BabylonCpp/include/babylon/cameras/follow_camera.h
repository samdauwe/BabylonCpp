#ifndef BABYLON_CAMERAS_FOLLOW_CAMERA_H
#define BABYLON_CAMERAS_FOLLOW_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/target_camera.h>

namespace BABYLON {

class FollowCamera;
using FollowCameraPtr = std::shared_ptr<FollowCamera>;

/**
 * @brief A follow camera takes a mesh as a target and follows it as it moves.
 * Both a free camera version followCamera and an arc rotate version
 * arcFollowCamera are available.
 * @see http://doc.babylonjs.com/features/cameras#follow-camera
 */
class BABYLON_SHARED_EXPORT FollowCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static FollowCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<FollowCamera>(
      new FollowCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~FollowCamera() override;

  Type type() const override;

  /**
   * @brief Hidden
   */
  void _checkInputs() override;

  /**
   * @brief Gets the camera class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

  json serialize() const override;

protected:
  /**
   * @brief Instantiates the follow camera.
   * @see http://doc.babylonjs.com/features/cameras#follow-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the position of the camera
   * @param scene Define the scene the camera belong to
   * @param lockedTarget Define the target of the camera
   */
  FollowCamera(const std::string& name, const Vector3& position, Scene* scene,
               const AbstractMeshPtr& iLockedTarget = nullptr);

private:
  void _follow(const AbstractMeshPtr& cameraTarget);

public:
  /**
   * Distance the follow camera should follow an object at
   */
  float radius;

  /**
   * Define a rotation offset between the camera and the object it follows
   */
  float rotationOffset;

  /**
   * Define a height offset between the camera and the object it follows.
   * It can help following an object from the top (like a car chaing a plane)
   */
  float heightOffset;

  /**
   * Define how fast the camera can accelerate to follow it s target.
   */
  float cameraAcceleration;

  /**
   * Define the speed limit of the camera following an object.
   */
  float maxCameraSpeed;

  /**
   * Define the target of the camera.
   */
  AbstractMeshPtr lockedTarget;

  static bool NodeConstructorAdded;

}; // end of class FollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FOLLOW_CAMERA_H
