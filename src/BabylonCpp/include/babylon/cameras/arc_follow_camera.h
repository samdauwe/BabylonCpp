#ifndef BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H
#define BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class ArcFollowCamera;
using ArcFollowCameraPtr = std::shared_ptr<ArcFollowCamera>;

/**
 * @brief Arc Rotate version of the follow camera.
 * It still follows a Defined mesh but in an Arc Rotate Camera fashion.
 * @see https://doc.babylonjs.com/features/cameras#follow-camera
 */
class BABYLON_SHARED_EXPORT ArcFollowCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static ArcFollowCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<ArcFollowCamera>(new ArcFollowCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~ArcFollowCamera() override; // = default

  Type type() const override;

  /**
   * @brief Hidden
   */
  void _checkInputs() override;

  /**
   * @brief Returns the class name of the object.
   * It is mostly used internally for serialization purposes.
   */
  std::string getClassName() const override;

protected:
  /**
   * @brief Instantiates a new ArcFollowCamera.
   * @see https://doc.babylonjs.com/features/cameras#follow-camera
   * @param name Define the name of the camera
   * @param alpha Define the rotation angle of the camera around the logitudinal axis
   * @param beta Define the rotation angle of the camera around the elevation axis
   * @param radius Define the radius of the camera from its target point
   * @param target Define the target of the camera
   * @param scene Define the scene the camera belongs to
   */
  ArcFollowCamera(const std::string& name, float alpha, float beta, float radius,
                  AbstractMesh* target, Scene* scene);

private:
  void _follow();

public:
  /**
   * The longitudinal angle of the camera
   */

  float alpha;
  /**
   * The latitudinal angle of the camera
   */

  float beta;
  /**
   * The radius of the camera from its target
   */

  float radius;

  /**
   * Define the camera target (the mesh it should follow)
   */
  AbstractMesh* target;

private:
  Vector3 _cartesianCoordinates;

  static bool NodeConstructorAdded;

}; // end of class ArcFollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_ARC_FOLLOW_CAMERA_H
