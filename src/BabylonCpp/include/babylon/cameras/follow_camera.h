#ifndef BABYLON_CAMERAS_FOLLOW_CAMERA_H
#define BABYLON_CAMERAS_FOLLOW_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/target_camera.h>

namespace BABYLON {

class FollowCamera;
using FollowCameraPtr = std::shared_ptr<FollowCamera>;

class BABYLON_SHARED_EXPORT FollowCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static FollowCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<FollowCamera>(
      new FollowCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~FollowCamera() override;

  IReflect::Type type() const override;

  /** Hidden */
  void _checkInputs() override;
  const std::string getClassName() const override;
  Json::object serialize() const override;

protected:
  FollowCamera(const std::string& name, const Vector3& position, Scene* scene,
               AbstractMesh* iLockedTarget = nullptr);

private:
  float getRadians(float degrees) const;
  void follow(AbstractMesh* cameraTarget);

public:
  float radius;
  float rotationOffset;
  float heightOffset;
  float cameraAcceleration;
  float maxCameraSpeed;
  AbstractMesh* lockedTarget;

  static bool NodeConstructorAdded;

}; // end of class FollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FOLLOW_CAMERA_H
