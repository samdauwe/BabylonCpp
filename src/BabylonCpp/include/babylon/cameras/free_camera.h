#ifndef BABYLON_CAMERAS_FREE_CAMERA_H
#define BABYLON_CAMERAS_FREE_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera_inputs_manager.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class Collider;
class FreeCamera;
using FreeCameraPtr = std::shared_ptr<FreeCamera>;

class BABYLON_SHARED_EXPORT FreeCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static FreeCameraPtr New(Ts&&... args)
  {
    auto camera
      = std::shared_ptr<FreeCamera>(new FreeCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~FreeCamera() override;

  IReflect::Type type() const override;

  /** Controls **/
  void attachControl(ICanvas* canvas, bool noPreventDefault = false,
                     bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton
                     = MouseButtonType::RIGHT) override;
  void detachControl(ICanvas* canvas) override;
  /** Hidden */
  void _collideWithWorld(Vector3& displacement);
  /** Hidden */
  void _checkInputs() override;
  /** Hidden */
  bool _decideIfNeedsToMove() override;
  /** Hidden */
  void _updatePosition() override;
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;
  const std::string getClassName() const override;
  json serialize() const override;

private:
  int get_collisionMask() const;
  void set_collisionMask(int mask);
  void _onCollisionPositionChange(int collisionId, Vector3& newPosition,
                                  AbstractMesh* collidedMesh = nullptr);

protected:
  FreeCamera(const std::string& name, const Vector3& position, Scene* scene,
             bool setActiveOnSceneIfNoneActive = true);

public:
  Vector3 ellipsoid;
  Vector3 ellipsoidOffset;
  bool checkCollisions;
  bool applyGravity;
  std::unique_ptr<FreeCameraInputsManager> inputs;
  // Collisions
  std::function<void(AbstractMesh* collidedMesh)> onCollide;
  // Direction
  /** Hidden */
  std::unique_ptr<Vector3> _localDirection;
  /** Hidden */
  Vector3 _transformedDirection;

  Property<FreeCamera, int> collisionMask;

private:
  // Collisions
  std::unique_ptr<Collider> _collider;
  int _collisionMask;
  bool _needMoveForGravity;
  Vector3 _oldPosition;
  Vector3 _diffPosition;
  Vector3 _newPosition;

  static bool NodeConstructorAdded;

}; // end of class FreeCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FREE_CAMERA_H
