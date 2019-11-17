#ifndef BABYLON_CAMERAS_TARGET_CAMERA_H
#define BABYLON_CAMERAS_TARGET_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

class TargetCamera;
using TargetCameraPtr = std::shared_ptr<TargetCamera>;

/**
 * @brief A target camera takes a mesh or position as a target and continues to look at it while it
 * moves. This is the base of the follow, arc rotate cameras and Free camera
 * @see http://doc.babylonjs.com/features/cameras
 */
class BABYLON_SHARED_EXPORT TargetCamera : public Camera {

public:
  template <typename... Ts>
  static TargetCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<TargetCamera>(new TargetCamera(std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~TargetCamera() override; // = default

  Type type() const override;

  /**
   * @brief Gets a property.
   */
  AnimationValue getProperty(const std::vector<std::string>& targetPropertyPath) override;

  /**
   * @brief Sets a property.
   */
  void setProperty(const std::vector<std::string>& targetPropertyPath,
                   const AnimationValue& value) override;

  /**
   * @brief Gets the position in front of the camera at a given distance.
   * @param distance The distance from the camera we want the position to be
   * @returns the position
   */
  Vector3 getFrontPosition(float distance);

  /**
   * @brief Hidden
   */
  Vector3* _getLockedTargetPosition();

  /**
   * @brief Store current camera state of the camera (fov, position, rotation, etc..)
   * @returns the camera
   */
  Camera& storeState() override;

  /**
   * @brief Restored camera state. You must call storeState() first.
   * @returns whether it was successful or not
   * Hidden
   */
  bool _restoreStateValues() override;

  /**
   * @brief Hidden
   */
  void _initCache() override;

  /**
   * @brief Hidden
   */
  void _updateCache(bool ignoreParentClass) override;

  /** Synchronized **/
  /**
   * @brief Hidden
   */
  bool _isSynchronizedViewMatrix() override;

  /** Methods **/
  /**
   * @brief Hidden
   */
  float _computeLocalCameraSpeed();

  /**
   * @brief Defines the target the camera should look at.
   * @param target Defines the new target as a Vector or a mesh
   */
  void setTarget(const Vector3& target);

  /**
   * @brief Return the current target position of the camera. This value is expressed in local
   * space.
   * @returns the target position
   */
  Vector3& getTarget();

  /**
   * @brief Hidden
   */
  virtual bool _decideIfNeedsToMove();

  /**
   * @brief Hidden
   */
  virtual void _updatePosition();

  /**
   * @brief Hidden
   */
  void _checkInputs() override;

  /**
   * @brief Hidden
   */
  Matrix _getViewMatrix() override;

  /**
   * @brief Hidden
   */
  CameraPtr createRigCamera(const std::string& name, int cameraIndex) override;

  /**
   * @brief Hidden
   */
  void _updateRigCameras() override;

  /**
   * @brief Gets the current object class name.
   * @return the class name
   */
  std::string getClassName() const override;

  json serialize() const override;

protected:
  /**
   * @brief Instantiates a target camera that takes a meshor position as a target and continues to
   * look at it while it moves. This is the base of the follow, arc rotate cameras and Free camera
   * @see http://doc.babylonjs.com/features/cameras
   * @param name Defines the name of the camera in the scene
   * @param position Defines the start position of the camera in the scene
   * @param scene Defines the scene the camera belongs to
   * @param setActiveOnSceneIfNoneActive Defines wheter the camera should be marked as active if not
   * other active cameras have been defined
   */
  TargetCamera(const std::string& name, const Vector3& position, Scene* scene,
               bool setActiveOnSceneIfNoneActive = true);

  void _computeViewMatrix(const Vector3& position, const Vector3& target, const Vector3& up);

  /**
   * @brief Get the current rotation of the camera.
   */
  Vector3& get_rotation();

  /**
   * @brief Set the current rotation of the camera.
   */
  void set_rotation(const Vector3& value);

private:
  /**
   * @brief Update the up vector to apply the rotation of the camera (So if you changed the camera
   * rotation.z this will let you update the up vector as well)
   * @returns the current camera
   */
  TargetCamera& _rotateUpVectorWithCameraRotationMatrix();

  void _getRigCamPositionAndTarget(float halfSpace, TargetCamera& rigCamera);
  void _updateCameraRotationMatrix() override;

public:
  /**
   * Define the current direction the camera is moving to
   */
  std::unique_ptr<Vector3> cameraDirection;

  /**
   * Define the current rotation the camera is rotating to
   */
  std::unique_ptr<Vector2> cameraRotation;

  /**
   * When set, the up vector of the camera will be updated by the rotation of the camera
   */
  bool updateUpVectorFromRotation;

  /**
   * Define the current rotation of the camera
   */
  Property<TargetCamera, Vector3> rotation;

  /**
   * Define the current rotation of the camera as a quaternion to prevent Gimbal lock
   */
  std::unique_ptr<Quaternion> rotationQuaternion;

  /**
   * Define the current speed of the camera
   */
  float speed;

  /**
   * Add cconstraint to the camera to prevent it to move freely in all directions and around all
   * axis.
   */
  bool noRotationConstraint;

  /**
   * Define the current target of the camera as an object or a position.
   */
  Vector3* lockedTarget;

  /**
   * Hidden
   */
  Vector3 _currentTarget;
  /**
   * Hidden
   */
  float _initialFocalDistance;
  /**
   * Hidden
   */
  Matrix _viewMatrix;
  /**
   * Hidden
   */
  Matrix _camMatrix;
  /**
   * Hidden
   */
  Matrix _cameraTransformMatrix;
  /**
   * Hidden
   */
  Matrix _cameraRotationMatrix;

  /**
   * Hidden
   */
  std::unique_ptr<Vector3> _referencePoint;
  /**
   * Hidden
   */
  Vector3 _transformedReferencePoint;

  /**
   * Hidden
   */
  std::function<void()> _reset;
  std::string _waitingLockedTargetId;

protected:
  Vector3 _globalCurrentTarget;
  Vector3 _globalCurrentUpVector;

private:
  static Matrix _RigCamTransformMatrix;
  static Matrix _TargetTransformMatrix;
  static Vector3 _TargetFocalPoint;
  Quaternion _tmpQuaternion;
  std::unique_ptr<Vector3> _rotation;
  Vector3 _defaultUp;
  float _cachedRotationZ;
  float _cachedQuaternionRotationZ;
  Vector3 _storedPosition;
  Vector3 _storedRotation;
  Quaternion _storedRotationQuaternion;

}; // end of class TargetCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_TARGET_CAMERA_H
