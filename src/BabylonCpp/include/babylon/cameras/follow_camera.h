#ifndef BABYLON_CAMERAS_FOLLOW_CAMERA_H
#define BABYLON_CAMERAS_FOLLOW_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/target_camera.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FollowCamera : public TargetCamera {

public:
  template <typename... Ts>
  static FollowCamera* New(Ts&&... args)
  {
    auto camera = new FollowCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<std::unique_ptr<Camera>>(camera));

    return camera;
  }
  ~FollowCamera();

  Node::Type type() const override;

  void _checkInputs() override;
  std::string getTypeName() const override;
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

}; // end of class FollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FOLLOW_CAMERA_H
