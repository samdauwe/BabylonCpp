#ifndef BABYLON_CAMERAS_FOLLOW_CAMERA_H
#define BABYLON_CAMERAS_FOLLOW_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/follow_camera_inputs_manager.h>
#include <babylon/cameras/target_camera.h>

namespace BABYLON {

class FollowCamera;
using FollowCameraPtr = std::shared_ptr<FollowCamera>;

/**
 * @brief A follow camera takes a mesh as a target and follows it as it moves.
 * Both a free camera version followCamera and an arc rotate version
 * arcFollowCamera are available.
 * @see http://doc.babylonjs.com/features/cameras#follow-camera
 */
class BABYLON_SHARED_EXPORT FollowCamera : public TargetCamera {

public:
  static void AddNodeConstructor();

public:
  template <typename... Ts>
  static FollowCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<FollowCamera>(new FollowCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~FollowCamera() override; // = default

  Type type() const override;

  /**
   * @brief Attached controls to the current camera.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* element, bool noPreventDefault = false, bool useCtrlForPanning = true,
                     MouseButtonType panningMouseButton = MouseButtonType::RIGHT) override;

  /**
   * @brief Detach the current controls from the camera.
   * The camera will stop reacting to inputs.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* element) override;

  /**
   * @brief Hidden
   */
  void _checkInputs() override;

  /**
   * @brief Gets the camera class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

  json serialize() const override;

protected:
  /**
   * @brief Instantiates the follow camera.
   * @see http://doc.babylonjs.com/features/cameras#follow-camera
   * @param name Define the name of the camera in the scene
   * @param position Define the position of the camera
   * @param scene Define the scene the camera belong to
   * @param lockedTarget Define the target of the camera
   */
  FollowCamera(const std::string& name, const Vector3& position, Scene* scene,
               const AbstractMeshPtr& iLockedTarget = nullptr);

private:
  void _follow(const AbstractMeshPtr& cameraTarget);
  void _checkLimits();

public:
  /**
   * Distance the follow camera should follow an object at
   */
  float radius;

  /**
   * Minimum allowed distance of the camera to the axis of rotation
   * (The camera can not get closer).
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> lowerRadiusLimit;

  /**
   * Maximum allowed distance of the camera to the axis of rotation
   * (The camera can not get further).
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> upperRadiusLimit;

  /**
   * Define a rotation offset between the camera and the object it follows
   */
  float rotationOffset;

  /**
   * Minimum allowed angle to camera position relative to target object.
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> lowerRotationOffsetLimit;

  /**
   * Maximum allowed angle to camera position relative to target object.
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> upperRotationOffsetLimit;

  /**
   * Define a height offset between the camera and the object it follows.
   * It can help following an object from the top (like a car chaing a plane)
   */
  float heightOffset;

  /**
   * Minimum allowed height of camera position relative to target object.
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> lowerHeightOffsetLimit;

  /**
   * Maximum allowed height of camera position relative to target object.
   * This can help limiting how the Camera is able to move in the scene.
   */
  std::optional<float> upperHeightOffsetLimit;

  /**
   * Define how fast the camera can accelerate to follow it s target.
   */
  float cameraAcceleration;

  /**
   * Define the speed limit of the camera following an object.
   */
  float maxCameraSpeed;

  /**
   * Define the target of the camera.
   */
  AbstractMeshPtr lockedTarget;

  /**
   * Defines the input associated with the camera.
   */
  std::unique_ptr<FollowCameraInputsManager> inputs;

  static bool NodeConstructorAdded;

}; // end of class FollowCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_FOLLOW_CAMERA_H
