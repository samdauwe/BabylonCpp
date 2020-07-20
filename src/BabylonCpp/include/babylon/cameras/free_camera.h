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
 * @brief This represents a free type of camera. It can be useful in First Person Shooter game for
 * instance. Please consider using the new UniversalCamera instead as it adds more functionality
 * like the gamepad.
 * @see https://doc.babylonjs.com/features/cameras#universal-camera
 */
class BABYLON_SHARED_EXPORT FreeCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static FreeCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<FreeCamera>(new FreeCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~FreeCamera() override; // = default

  Type type() const override;

  /**
   * @brief Attached controls to the current camera.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault = false, bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton = MouseButtonType::RIGHT) override;

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
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  std::string getClassName() const override;

  json serialize() const override;

private:
  int get_collisionMask() const;
  void set_collisionMask(int mask);
  void _onCollisionPositionChange(size_t collisionId, Vector3& newPosition,
                                  const AbstractMeshPtr& collidedMesh = nullptr);

protected:
  /**
   * @brief Instantiates a Free Camera.
   * This represents a free type of camera. It can be useful in First Person Shooter game for
   * instance. Please consider using the new UniversalCamera instead as it adds more functionality
   * like touch to this camera.
   * @see https://doc.babylonjs.com/features/cameras#universal-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the start position of the camera in the scene
   * @param scene Define the scene the camera belongs to
   * @param setActiveOnSceneIfNoneActive Defines wheter the camera should be marked as active if not
   * other active cameras have been defined
   */
  FreeCamera(const std::string& name, const Vector3& position, Scene* scene,
             bool setActiveOnSceneIfNoneActive = true);

  /**
   * @brief Gets the input sensibility for a mouse input. (default is 2000.0)
   * Higher values reduce sensitivity.
   */
  float get_angularSensibility() const;

  /**
   * @brief Sets the input sensibility for a mouse input. (default is 2000.0)
   * Higher values reduce sensitivity.
   */
  void set_angularSensibility(float value);

  /**
   * @brief Gets the list of keyboard keys used to control the forward move of the camera.
   */
  Int32Array& get_keysUp();

  /**
   * @brief Sets the list of keyboard keys used to control the forward move of the camera.
   */
  void set_keysUp(const Int32Array& value);

  /**
   * @brief Gets the list of keyboard keys used to control the upward move of the camera.
   */
  Int32Array& get_keysUpward();

  /**
   * @brief Sets the list of keyboard keys used to control the upward move of the camera.
   */
  void set_keysUpward(const Int32Array& value);

  /**
   * @brief Gets the list of keyboard keys used to control the backward move of the camera.
   */
  Int32Array& get_keysDown();

  /**
   * @brief Sets the list of keyboard keys used to control the backward move of the camera.
   */
  void set_keysDown(const Int32Array& value);

  /**
   *@brief Gets the list of keyboard keys used to control the downward move of the camera.
   */
  Int32Array& get_keysDownward();

  /**
   * @brief Sets the list of keyboard keys used to control the backward move of the camera.
   */
  void set_keysDownward(const Int32Array& value);

  /**
   * @brief Gets the list of keyboard keys used to control the left strafe move of the camera.
   */
  Int32Array& get_keysLeft();

  /**
   * @brief Sets the list of keyboard keys used to control the left strafe move of the camera.
   */
  void set_keysLeft(const Int32Array& value);

  /**
   * @brief Gets the list of keyboard keys used to control the right strafe move of the camera.
   */
  Int32Array& get_keysRight();

  /**
   * @brief Sets the list of keyboard keys used to control the right strafe move of the camera.
   */
  void set_keysRight(const Int32Array& value);

public:
  /**
   * Define the collision ellipsoid of the camera.
   * This is helpful to simulate a camera body like the player body around the camera
   * @see https://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity#arcrotatecamera
   */
  Vector3 ellipsoid;

  /**
   * Define an offset for the position of the ellipsoid around the camera.
   * This can be helpful to determine the center of the body near the gravity center of the body
   * instead of its head.
   */
  Vector3 ellipsoidOffset;

  /**
   * Enable or disable collisions of the camera with the rest of the scene objects.
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
   * The input sensibility for a mouse input. (default is 2000.0)
   * Higher values reduce sensitivity.
   */
  Property<FreeCamera, float> angularSensibility;

  /**
   * Gets or Set the list of keyboard keys used to control the forward move of the camera.
   */
  Property<FreeCamera, Int32Array> keysUp;

  /**
   * Gets or Set the list of keyboard keys used to control the upward move of the camera.
   */
  Property<FreeCamera, Int32Array> keysUpward;

  /**
   * Gets or Set the list of keyboard keys used to control the backward move of the camera.
   */
  Property<FreeCamera, Int32Array> keysDown;

  /**
   * Gets or Set the list of keyboard keys used to control the downward move of the camera.
   */
  Property<FreeCamera, Int32Array> keysDownward;

  /**
   * Gets or Set the list of keyboard keys used to control the left strafe move of the camera.
   */
  Property<FreeCamera, Int32Array> keysLeft;

  /**
   * Gets or Set the list of keyboard keys used to control the right strafe move of the camera.
   */
  Property<FreeCamera, Int32Array> keysRight;

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
   * Define a collision mask to limit the list of object the camera can collide with
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
  Int32Array _emptyKeyboardKeys;

  static bool NodeConstructorAdded;

}; // end of class FreeCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FREE_CAMERA_H
