#ifndef BABYLON_CAMERAS_FREE_CAMERA_H
#define BABYLON_CAMERAS_FREE_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/free_camera_inputs_manager.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class Collider;
class FreeCamera;
using ColliderPtr   = std::shared_ptr<Collider>;
using FreeCameraPtr = std::shared_ptr<FreeCamera>;

/**
 * @brief This represents a free type of camera. It can be usefull in First
 * Person Shooter game for instance. Please consider using the new
 * UniversalCamera instead as it adds more functionality like the gamepad.
 * @see http://doc.babylonjs.com/features/cameras#universal-camera
 */
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

  Type type() const override;

  /**
   * @brief Attached controls to the current camera.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault = false,
                     bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton
                     = MouseButtonType::RIGHT) override;

  /**
   * @brief Detach the current controls from the camera.
   * The camera will stop reacting to inputs.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* canvas) override;

  /**
   * @brief Hidden
   */
  void _collideWithWorld(Vector3& displacement);

  /**
   * @brief Hidden
   */
  void _checkInputs() override;

  /**
   * @brief Hidden
   */
  bool _decideIfNeedsToMove() override;

  /**
   * @brief Hidden
   */
  void _updatePosition() override;

  /**
   * @brief Destroy the camera and release the current resources hold by it.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  const std::string getClassName() const override;

  json serialize() const override;

private:
  int get_collisionMask() const;
  void set_collisionMask(int mask);
  void _onCollisionPositionChange(int collisionId, Vector3& newPosition,
                                  const AbstractMeshPtr& collidedMesh
                                  = nullptr);

protected:
  /**
   * @brief Instantiates a Free Camera.
   * This represents a free type of camera. It can be usefull in First Person
   * Shooter game for instance. Please consider using the new UniversalCamera
   * instead as it adds more functionality like touch to this camera.
   * @see http://doc.babylonjs.com/features/cameras#universal-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the start position of the camera in the scene
   * @param scene Define the scene the camera belongs to
   * @param setActiveOnSceneIfNoneActive Defines wheter the camera should be
   * marked as active if not other active cameras have been defined
   */
  FreeCamera(const std::string& name, const Vector3& position, Scene* scene,
             bool setActiveOnSceneIfNoneActive = true);

public:
  /**
   * Define the collision ellipsoid of the camera.
   * This is helpful to simulate a camera body like the player body around the
   * camera
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity#arcrotatecamera
   */
  Vector3 ellipsoid;

  /**
   * Define an offset for the position of the ellipsoid around the camera.
   * This can be helpful to determine the center of the body near the gravity
   * center of the body instead of its head.
   */
  Vector3 ellipsoidOffset;

  /**
   * Enable or disable collisions of the camera with the rest of the scene
   * objects.
   */
  bool checkCollisions;

  /**
   * Enable or disable gravity on the camera.
   */
  bool applyGravity;

  /**
   * Define the input manager associated to the camera.
   */
  std::unique_ptr<FreeCameraInputsManager> inputs;

  /**
   * Event raised when the camera collide with a mesh in the scene.
   */
  std::function<void(const AbstractMeshPtr& collidedMesh)> onCollide;

  /**
   * Hidden
   */
  std::unique_ptr<Vector3> _localDirection;

  /**
   * Hidden
   */
  Vector3 _transformedDirection;

  /**
   * Define a collision mask to limit the list of object the camera can collide
   * with
   */
  Property<FreeCamera, int> collisionMask;

private:
  // Collisions
  ColliderPtr _collider;
  int _collisionMask;
  bool _needMoveForGravity;
  Vector3 _oldPosition;
  Vector3 _diffPosition;
  Vector3 _newPosition;

  static bool NodeConstructorAdded;

}; // end of class FreeCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FREE_CAMERA_H
