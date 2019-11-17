#ifndef BABYLON_CAMERAS_FLY_CAMERA_H
#define BABYLON_CAMERAS_FLY_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/target_camera.h>

namespace BABYLON {

class Collider;
class FlyCamera;
struct FlyCameraInputsManager;
class ICanvas;
using ColliderPtr  = std::shared_ptr<Collider>;
using FlyCameraPtr = std::shared_ptr<FlyCamera>;

/**
 * @brief This is a flying camera, designed for 3D movement and rotation in all
 * directions, such as in a 3D Space Shooter or a Flight Simulator.
 */
class BABYLON_SHARED_EXPORT FlyCamera : public TargetCamera {

public:
  template <typename... Ts>
  static FlyCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<FlyCamera>(new FlyCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~FlyCamera() override; // = default

  /**
   * @brief Attach a control to the HTML DOM element.
   * @param element Defines the element that listens to the input events.
   * @param noPreventDefault Defines whether events caught by the controls
   * should call preventdefault().
   * https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault
   */
  void attachControl(ICanvas* element, bool noPreventDefault = false, bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton = MouseButtonType::RIGHT) override;

  /**
   * @brief Detach a control from the HTML DOM element.
   * The camera will stop reacting to that input.
   * @param element Defines the element that listens to the input events.
   */
  void detachControl(ICanvas* element) override;

  /**
   * @brief Hidden
   */
  void _collideWithWorld(const Vector3& displacement);

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
   * @brief Restore the Roll to its target value at the rate specified.
   * @param rate - Higher means slower restoring.
   * @hidden
   */
  void restoreRoll(float rate);

  /**
   * @brief Destroy the camera and release the current resources held by it.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Get the current object class name.
   * @returns the class name.
   */
  const std::string getClassName() const override;

protected:
  /**
   * @brief Instantiates a FlyCamera.
   * This is a flying camera, designed for 3D movement and rotation in all
   * directions, such as in a 3D Space Shooter or a Flight Simulator.
   * @param name Define the name of the camera in the scene.
   * @param position Define the starting position of the camera in the scene.
   * @param scene Define the scene the camera belongs to.
   * @param setActiveOnSceneIfNoneActive Defines wheter the camera should be
   * marked as active, if no other camera has been defined as active.
   */
  FlyCamera(const std::string& name, const Vector3& position, Scene* scene,
            bool setActiveOnSceneIfNoneActive = true);

  /**
   * @brief Gets the input sensibility for mouse input.
   * Higher values reduce sensitivity.
   */
  float get_angularSensibility() const;

  /**
   * @brief Sets the input sensibility for a mouse input.
   * Higher values reduce sensitivity.
   */
  void set_angularSensibility(float value);

  /**
   * @brief Get the keys for camera movement forward.
   */
  Int32Array& get_keysForward();

  /**
   * @brief Set the keys for camera movement forward.
   */
  void set_keysForward(const Int32Array& value);

  /**
   * @brief Get the keys for camera movement backward.
   */
  Int32Array& get_keysBackward();

  /**
   * @brief Set the keys for camera movement backward.
   */
  void set_keysBackward(const Int32Array& value);

  /**
   * @brief Get the keys for camera movement up.
   */
  Int32Array& get_keysUp();

  /**
   * @brief Set the keys for camera movement up.
   */
  void set_keysUp(const Int32Array& value);

  /**
   * @brief Get the keys for camera movement down.
   */
  Int32Array& get_keysDown();

  /**
   * @brief Set the keys for camera movement down.
   */
  void set_keysDown(const Int32Array& value);

  /**
   * @brief Get the keys for camera movement left.
   */
  Int32Array& get_keysLeft();

  /**
   * @brief Set the keys for camera movement left.
   */
  void set_keysLeft(const Int32Array& value);

  /**
   * @brief Set the keys for camera movement right.
   */
  Int32Array& get_keysRight();

  /**
   * @brief Set the keys for camera movement right.
   */
  void set_keysRight(const Int32Array& value);

  /**
   * @brief Get the mask that the camera ignores in collision events.
   */
  int get_collisionMask() const;

  /**
   * @brief Set the mask that the camera ignores in collision events.
   */
  void set_collisionMask(int mask);

private:
  /**
   * @brief Hidden
   */
  void _onCollisionPositionChange(size_t collisionId, const Vector3& newPosition,
                                  const AbstractMeshPtr& collidedMesh = nullptr);

public:
  /**
   * Define the collision ellipsoid of the camera.
   * This is helpful for simulating a camera body, like a player's body.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity#arcrotatecamera
   */
  Vector3 ellipsoid;

  /**
   * Define an offset for the position of the ellipsoid around the camera.
   * This can be helpful if the camera is attached away from the player's body
   * center, such as at its head.
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
   * Track Roll to maintain the wanted Rolling when looking around.
   */
  float _trackRoll;

  /**
   * Slowly correct the Roll to its original value after a Pitch+Yaw rotation.
   */
  float rollCorrect;

  /**
   * Mimic a banked turn, Rolling the camera when Yawing.
   * It's recommended to use rollCorrect = 10 for faster banking correction.
   */
  bool bankedTurn;

  /**
   * Limit in radians for how much Roll banking will add. (Default: 90°)
   */
  float bankedTurnLimit;

  /**
   * Value of 0 disables the banked Roll.
   * Value of 1 is equal to the Yaw angle in radians.
   */
  int bankedTurnMultiplier;

  /**
   * The inputs manager loads all the input sources, such as keyboard and mouse.
   */
  std::unique_ptr<FlyCameraInputsManager> inputs;

  /**
   * The input sensibility for mouse input.
   * Higher values reduce sensitivity.
   */
  Property<FlyCamera, float> angularSensibility;

  /**
   * The keys for camera movement forward.
   */
  Property<FlyCamera, Int32Array> keysForward;

  /**
   * The keys for camera movement backward.
   */
  Property<FlyCamera, Int32Array> keysBackward;

  /**
   * The keys for camera movement up.
   */
  Property<FlyCamera, Int32Array> keysUp;

  /**
   * The keys for camera movement down.
   */
  Property<FlyCamera, Int32Array> keysDown;

  /**
   * The keys for camera movement left.
   */
  Property<FlyCamera, Int32Array> keysLeft;

  /**
   * The keys for camera movement right.
   */
  Property<FlyCamera, Int32Array> keysRight;

  /**
   * Event raised when the camera collides with a mesh in the scene.
   */
  std::function<void(const AbstractMeshPtr& collidedMesh)> onCollide;

  /** Hidden */
  std::optional<Vector3> _localDirection;
  /** Hidden */
  Vector3 _transformedDirection;

  /**
   * The mask that the camera ignores in collision events.
   */
  Property<FlyCamera, int> collisionMask;

private:
  ColliderPtr _collider;
  bool _needMoveForGravity;
  Vector3 _oldPosition;
  Vector3 _diffPosition;
  Vector3 _newPosition;
  // Collisions.
  int _collisionMask;
  Int32Array _emptyKeyboardKeys;

}; // end of class FlyCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FLY_CAMERA_H
