#ifndef BABYLON_CAMERAS_TARGET_CAMERA_H
#define BABYLON_CAMERAS_TARGET_CAMERA_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/camera.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT TargetCamera : public Camera {

public:
  template <typename... Ts>
  static TargetCamera* New(Ts&&... args)
  {
    auto camera = new TargetCamera(::std::forward<Ts>(args)...);
    camera->addToScene(static_cast<unique_ptr_t<Camera>>(camera));

    return camera;
  }
  ~TargetCamera() override;

  virtual IReflect::Type type() const override;

  Vector3 getFrontPosition(float distance);
  Vector3* _getLockedTargetPosition();

  /** State */
  Camera& storeState() override;

  /**
   * @brief Restored camera state. You must call storeState() first
   */
  bool _restoreStateValues() override;

  /** Cache */
  void _initCache() override;
  void _updateCache(bool ignoreParentClass) override;

  /** Synchronized **/
  bool _isSynchronizedProjectionMatrix();

  /** Methods **/
  float _computeLocalCameraSpeed();

  /** Target **/
  void setRotation(const Vector3& rotation);
  Vector3& getRotation();
  void setTarget(const Vector3& target);

  /**
   * @brief Return the current target position of the camera. This value is
   * expressed in local space.
   */
  Vector3& getTarget();

  virtual bool _decideIfNeedsToMove();
  virtual void _updatePosition();
  void _checkInputs() override;
  Matrix _getViewMatrix() override;

  /** Camera rigs section **/
  Camera* createRigCamera(const string_t& name, int cameraIndex) override;
  void _updateRigCameras() override;
  const char* getClassName() const override;
  virtual Json::object serialize() const override;

protected:
  TargetCamera(const string_t& name, const Vector3& position, Scene* scene);

private:
  void _getRigCamPosition(float halfSpace, Vector3& result);
  void _updateCameraRotationMatrix() override;

public:
  unique_ptr_t<Vector3> cameraDirection;
  unique_ptr_t<Vector2> cameraRotation;
  unique_ptr_t<Vector3> rotation;

  unique_ptr_t<Quaternion> rotationQuaternion;

  float speed;
  bool noRotationConstraint;
  Vector3* lockedTarget;

  Vector3 _currentTarget;
  Matrix _viewMatrix;
  Matrix _camMatrix;
  Matrix _cameraTransformMatrix;
  Matrix _cameraRotationMatrix;
  unique_ptr_t<Vector3> _referencePoint;
  Vector3 _transformedReferencePoint;
  Matrix _lookAtTemp;
  Matrix _tempMatrix;

  ::std::function<void()> _reset;
  string_t _waitingLockedTargetId;

private:
  Matrix _rigCamTransformMatrix;
  unique_ptr_t<Vector3> _currentUpVector;
  /**
   * Store current camera state (fov, position, etc..)
   */
  Vector3 _storedPosition;
  Vector3 _storedRotation;
  Quaternion _storedRotationQuaternion;

}; // end of class TargetCamera

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_TARGET_CAMERA_H
