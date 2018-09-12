#ifndef BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H
#define BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class ArcFollowCamera;
using ArcFollowCameraPtr = std::shared_ptr<ArcFollowCamera>;

class BABYLON_SHARED_EXPORT ArcFollowCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static ArcFollowCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<ArcFollowCamera>(
      new ArcFollowCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~ArcFollowCamera() override;

  virtual IReflect::Type type() const override;

  /** Hidden */
  void _checkInputs() override;
  const std::string getClassName() const override;

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

  static bool NodeConstructorAdded;

}; // end of class ArcFollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H
