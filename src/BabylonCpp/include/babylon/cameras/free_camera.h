#ifndef BABYLON_CAMERAS_FREE_CAMERA_H
#define BABYLON_CAMERAS_FREE_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/free_camera_inputs_manager.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FreeCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static FreeCamera* New(Ts&&... args)
  {
    auto camera = new FreeCamera(std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

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
  void _collideWithWorld(Vector3& displacement);
  void _checkInputs() override;
  bool _decideIfNeedsToMove() override;
  void _updatePosition() override;
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;
  const string_t getClassName() const override;
  Json::object serialize() const override;

private:
  int get_collisionMask() const;
  void set_collisionMask(int mask);
  void _onCollisionPositionChange(int collisionId, Vector3& newPosition,
                                  AbstractMesh* collidedMesh = nullptr);

protected:
  FreeCamera(const string_t& name, const Vector3& position, Scene* scene,
             bool setActiveOnSceneIfNoneActive = true);

public:
  Vector3 ellipsoid;
  Vector3 ellipsoidOffset;
  bool checkCollisions;
  bool applyGravity;
  unique_ptr_t<FreeCameraInputsManager> inputs;
  // Collisions
  ::std::function<void(AbstractMesh* collidedMesh)> onCollide;
  // Direction
  unique_ptr_t<Vector3> _localDirection;
  Vector3 _transformedDirection;

  Property<FreeCamera, int> collisionMask;

private:
  // Collisions
  unique_ptr_t<Collider> _collider;
  int _collisionMask;
  bool _needMoveForGravity;
  Vector3 _oldPosition;
  Vector3 _diffPosition;
  Vector3 _newPosition;

  static bool NodeConstructorAdded;

}; // end of class FreeCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FREE_CAMERA_H
