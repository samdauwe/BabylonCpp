#ifndef BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H
#define BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ArcFollowCamera : public TargetCamera {

public:
  template <typename... Ts>
  static ArcFollowCamera* New(Ts&&... args)
  {
    auto camera = new ArcFollowCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<std::unique_ptr<Camera>>(camera));

    return camera;
  }
  ~ArcFollowCamera();

  virtual IReflect::Type type() const override;

  void _checkInputs() override;
  std::string getTypeName() const override;

protected:
  ArcFollowCamera(const std::string& name, float alpha, float beta,
                  float radius, AbstractMesh* target, Scene* scene);

private:
  void follow();

public:
  float alpha;
  float beta;
  float radius;
  AbstractMesh* target;

private:
  Vector3 _cartesianCoordinates;

}; // end of class ArcFollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H
