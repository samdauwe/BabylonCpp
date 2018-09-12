#ifndef BABYLON_CAMERAS_TARGET_CAMERA_H
#define BABYLON_CAMERAS_TARGET_CAMERA_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

class TargetCamera;
using TargetCameraPtr = std::shared_ptr<TargetCamera>;

class BABYLON_SHARED_EXPORT TargetCamera : public Camera {

public:
  template <typename... Ts>
  static TargetCameraPtr New(Ts&&... args)
  {
    auto camera = std::shared_ptr<TargetCamera>(
      new TargetCamera(::std::forward<Ts>(args)...));
    camera->addToScene(camera);

    return camera;
  }
  ~TargetCamera() override;

  virtual IReflect::Type type() const override;

  Vector3 getFrontPosition(float distance);
  /** Hidden */
  Vector3* _getLockedTargetPosition();

  /** State */
  Camera& storeState() override;

  /**
   * @brief Restored camera state. You must call storeState() first.
   * @returns whether it was successful or not
   * Hidden
   */
  bool _restoreStateValues() override;

  /** Cache */
  /** Hidden */
  void _initCache() override;
  /** Hidden */
  void _updateCache(bool ignoreParentClass) override;

  /** Synchronized **/
  /** Hidden */
  bool _isSynchronizedViewMatrix() override;

  /** Methods **/
  /** Hidden */
  float _computeLocalCameraSpeed();

  /** Target **/
  void setRotation(const Vector3& rotation);
  Vector3& getRotation();
  /** Hidden */
  void setTarget(const Vector3& target);

  /**
   * @brief Return the current target position of the camera. This value is
   * expressed in local space.
   */
  Vector3& getTarget();

  /** Hidden */
  virtual bool _decideIfNeedsToMove();
  /** Hidden */
  virtual void _updatePosition();
  /** Hidden */
  void _checkInputs() override;
  /** Hidden */
  Matrix _getViewMatrix() override;

  /** Camera rigs section **/
  CameraPtr createRigCamera(const std::string& name, int cameraIndex) override;
  /** Hidden */
  void _updateRigCameras() override;
  const std::string getClassName() const override;
  virtual Json::object serialize() const override;

protected:
  TargetCamera(const std::string& name, const Vector3& position, Scene* scene,
               bool setActiveOnSceneIfNoneActive = true);

  void _computeViewMatrix(const Vector3& position, const Vector3& target,
                          const Vector3& up);

private:
  /**
   * @brief Update the up vector to apply the rotation of the camera (So if you
   * changed the camera rotation.z this will let you update the up vector as
   * well).
   * @returns the current camera
   */
  TargetCamera& _rotateUpVectorWithCameraRotationMatrix();

  void _getRigCamPosition(float halfSpace, Vector3& result);
  void _updateCameraRotationMatrix() override;

public:
  std::unique_ptr<Vector3> cameraDirection;
  std::unique_ptr<Vector2> cameraRotation;
  std::unique_ptr<Vector3> rotation;

  std::unique_ptr<Quaternion> rotationQuaternion;

  float speed;
  bool noRotationConstraint;
  Vector3* lockedTarget;

  /** Hidden */
  Vector3 _currentTarget;
  /** Hidden */
  Matrix _viewMatrix;
  /** Hidden */
  Matrix _camMatrix;
  /** Hidden */
  Matrix _cameraTransformMatrix;
  /** Hidden */
  Matrix _cameraRotationMatrix;
  /** Hidden */
  std::unique_ptr<Vector3> _referencePoint;
  /** Hidden */
  Vector3 _transformedReferencePoint;
  /** Hidden */
  ::std::function<void()> _reset;
  std::string _waitingLockedTargetId;

protected:
  Vector3 _globalCurrentTarget;
  Vector3 _globalCurrentUpVector;

private:
  Matrix _rigCamTransformMatrix;
  Vector3 _defaultUp;
  float _cachedRotationZ;
  /**
   * Store current camera state (fov, position, etc..)
   */
  Vector3 _storedPosition;
  Vector3 _storedRotation;
  Quaternion _storedRotationQuaternion;

}; // end of class TargetCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_TARGET_CAMERA_H
