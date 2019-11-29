#include <babylon/cameras/target_camera.h>

#include <nlohmann/json.hpp>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/core/time.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/maths/axis.h>
#include <babylon/maths/tmp_vectors.h>

namespace BABYLON {

Matrix TargetCamera::_RigCamTransformMatrix;
Matrix TargetCamera::_TargetTransformMatrix;
Vector3 TargetCamera::_TargetFocalPoint;

TargetCamera::TargetCamera(const std::string& iName, const Vector3& iPosition, Scene* scene,
                           bool setActiveOnSceneIfNoneActive)
    : Camera(iName, iPosition, scene, setActiveOnSceneIfNoneActive)
    , cameraDirection{std::make_unique<Vector3>(0.f, 0.f, 0.f)}
    , cameraRotation{std::make_unique<Vector2>(0.f, 0.f)}
    , updateUpVectorFromRotation{false}
    , rotation{this, &TargetCamera::get_rotation, &TargetCamera::set_rotation}
    , rotationQuaternion{nullptr}
    , speed{2.f}
    , noRotationConstraint{false}
    , lockedTarget{nullptr}
    , _currentTarget{Vector3::Zero()}
    , _initialFocalDistance{1.f}
    , _viewMatrix{Matrix::Zero()}
    , _camMatrix{Matrix::Zero()}
    , _cameraTransformMatrix{Matrix::Zero()}
    , _cameraRotationMatrix{Matrix::Zero()}
    , _referencePoint{std::make_unique<Vector3>(0.f, 0.f, 1.f)}
    , _transformedReferencePoint{Vector3::Zero()}
    , _globalCurrentTarget{Vector3::Zero()}
    , _globalCurrentUpVector{Vector3::Zero()}
    , _rotation{std::make_unique<Vector3>(0.f, 0.f, 0.f)}
    , _defaultUp{Vector3::Up()}
    , _cachedRotationZ{0.f}
    , _cachedQuaternionRotationZ{0.f}
{
  // initCache() is a virtual method and cannot be called during construction
  // instead, it is called inside Camera::addToScene(), which is called by TargetCamera::New()
  // _initCache();
}

TargetCamera::~TargetCamera() = default;

Type TargetCamera::type() const
{
  return Type::TARGETCAMERA;
}

AnimationValue TargetCamera::getProperty(const std::vector<std::string>& targetPropertyPath)
{
  if (targetPropertyPath.size() == 1) {
    const auto& target = targetPropertyPath[0];
    if (target == "position") {
      return position();
    }
    if (target == "rotation") {
      return rotation();
    }
  }
  else if (targetPropertyPath.size() == 2) {
    const auto& target = targetPropertyPath[0];
    const auto& key    = targetPropertyPath[1];
    // Position
    if (target == "position") {
      return IAnimatable::getProperty(key, position());
    }
    // Rotation
    if (target == "rotation") {
      return IAnimatable::getProperty(key, rotation());
    }
  }

  return AnimationValue();
}

void TargetCamera::setProperty(const std::vector<std::string>& targetPropertyPath,
                               const AnimationValue& value)
{
  const auto animationType = value.animationType();
  if (animationType.has_value()) {
    if (targetPropertyPath.size() == 1) {
      const auto& target = targetPropertyPath[0];
      if (*animationType == Animation::ANIMATIONTYPE_VECTOR3()) {
        auto vector3Value = value.get<Vector3>();
        // Position
        if (target == "position") {
          position = vector3Value;
        }
        // Rotation
        if (target == "rotation") {
          rotation = vector3Value;
        }
      }
    }
    if (targetPropertyPath.size() == 2) {
      const auto& target = targetPropertyPath[0];
      const auto& key    = targetPropertyPath[1];
      if (*animationType == Animation::ANIMATIONTYPE_FLOAT()) {
        const auto& floatValue = value.get<float>();
        // Position
        if (target == "position") {
          IAnimatable::setProperty(key, position(), floatValue);
        }
        // Rotation
        if (target == "rotation") {
          IAnimatable::setProperty(key, rotation(), floatValue);
        }
      }
    }
  }
}

Vector3 TargetCamera::getFrontPosition(float distance)
{
  getWorldMatrix();
  auto direction = getTarget().subtract(position);
  direction.normalize();
  direction.scaleInPlace(distance);
  return globalPosition().add(direction);
}

Vector3* TargetCamera::_getLockedTargetPosition()
{
  if (!lockedTarget) {
    return nullptr;
  }

  return lockedTarget;
}

/** State */

Camera& TargetCamera::storeState()
{
  _storedPosition = position().copy();
  _storedRotation = rotation().copy();
  if (rotationQuaternion) {
    _storedRotationQuaternion = rotationQuaternion->copy();
  }

  return Camera::storeState();
}

bool TargetCamera::_restoreStateValues()
{
  if (!Camera::_restoreStateValues()) {
    return false;
  }

  position = _storedPosition.copy();
  rotation = _storedRotation.copy();

  if (rotationQuaternion) {
    rotationQuaternion = _storedRotationQuaternion.clone();
  }

  cameraDirection->copyFromFloats(0.f, 0.f, 0.f);
  cameraRotation->copyFromFloats(0.f, 0.f);

  return true;
}

/** Cache */
void TargetCamera::_initCache()
{
  Camera::_initCache();
  _cache.lockedTarget = std::make_unique<Vector3>(std::numeric_limits<float>::max(),
                                                  std::numeric_limits<float>::max(),
                                                  std::numeric_limits<float>::max());
  _cache.rotation = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max());
}

void TargetCamera::_updateCache(bool ignoreParentClass)
{
  if (!ignoreParentClass) {
    Camera::_updateCache(ignoreParentClass);
  }

  auto lockedTargetPosition = _getLockedTargetPosition();
  if (!lockedTargetPosition) {
    _cache.lockedTarget.reset(nullptr);
  }
  else {
    if (!_cache.lockedTarget) {
      _cache.lockedTarget.reset(lockedTargetPosition);
    }
    else {
      _cache.lockedTarget.get()->copyFrom(*lockedTargetPosition);
    }
  }

  _cache.rotation.copyFrom(rotation());
  if (rotationQuaternion) {
    _cache.rotationQuaternion.copyFrom(*rotationQuaternion);
  }
}

/** Synchronized **/
bool TargetCamera::_isSynchronizedViewMatrix()
{
  if (!Camera::_isSynchronizedViewMatrix()) {
    return false;
  }

  auto lockedTargetPosition = _getLockedTargetPosition();

  return (_cache.lockedTarget ? _cache.lockedTarget->equals(*lockedTargetPosition) :
                                !lockedTargetPosition)
         && (rotationQuaternion ? rotationQuaternion->equals(_cache.rotationQuaternion) :
                                  _cache.rotation.equals(rotation()));
}

/** Methods **/
float TargetCamera::_computeLocalCameraSpeed()
{
  auto engine = getEngine();

  return speed * std::sqrt((engine->getDeltaTime() / (engine->getFps() * 100.f)));
}

Vector3& TargetCamera::get_rotation()
{
  return *_rotation;
}

void TargetCamera::set_rotation(const Vector3& newRotation)
{
  _rotation = std::make_unique<Vector3>(newRotation);
}

/** Target **/
void TargetCamera::setTarget(const Vector3& target)
{
  upVector.normalize();

  _initialFocalDistance = target.subtract(position).length();

  if (stl_util::almost_equal(position().z, target.z)) {
    position().z += Math::Epsilon;
  }

  Matrix::LookAtLHToRef(position, target, _defaultUp, _camMatrix);
  _camMatrix.invert();

  _rotation->x = std::atan(_camMatrix.m()[6] / _camMatrix.m()[10]);

  auto vDir = target.subtract(position);

  if (vDir.x >= 0.f) {
    _rotation->y = (-std::atan(vDir.z / vDir.x) + Math::PI / 2.f);
  }
  else {
    _rotation->y = (-std::atan(vDir.z / vDir.x) - Math::PI / 2.f);
  }

  _rotation->z = 0.f;

  if (isNan(_rotation->x)) {
    _rotation->x = 0.f;
  }

  if (isNan(_rotation->y)) {
    _rotation->y = 0.f;
  }

  if (isNan(_rotation->z)) {
    _rotation->z = 0.f;
  }

  if (rotationQuaternion) {
    Quaternion::RotationYawPitchRollToRef(_rotation->y, _rotation->x, _rotation->z,
                                          *rotationQuaternion);
  }
}

Vector3& TargetCamera::getTarget()
{
  return _currentTarget;
}

bool TargetCamera::_decideIfNeedsToMove()
{
  return std::abs(cameraDirection->x) > 0.f || std::abs(cameraDirection->y) > 0.f
         || std::abs(cameraDirection->z) > 0.f;
}

void TargetCamera::_updatePosition()
{
  if (parent()) {
    parent()->getWorldMatrix().invertToRef(TmpVectors::MatrixArray[0]);
    Vector3::TransformNormalToRef(*cameraDirection, TmpVectors::MatrixArray[0],
                                  TmpVectors::Vector3Array[0]);
    position().addInPlace(TmpVectors::Vector3Array[0]);
    return;
  }
  position().addInPlace(*cameraDirection);
}

void TargetCamera::_checkInputs()
{
  bool needToMove   = _decideIfNeedsToMove();
  bool needToRotate = std::abs(cameraRotation->x) > 0.f || std::abs(cameraRotation->y) > 0.f;

  // Move
  if (needToMove) {
    _updatePosition();
  }

  // Rotate
  if (needToRotate) {
    _rotation->x += cameraRotation->x;
    _rotation->y += cameraRotation->y;

    // Rotate, if quaternion is set and rotation was used
    if (rotationQuaternion) {
      auto len = _rotation->lengthSquared();
      if (len > 0) {
        Quaternion::RotationYawPitchRollToRef(_rotation->y, _rotation->x, _rotation->z,
                                              *rotationQuaternion);
      }
    }

    if (!noRotationConstraint) {
      const auto limit = 1.570796f;

      if (_rotation->x > limit) {
        _rotation->x = limit;
      }
      if (_rotation->x < -limit) {
        _rotation->x = -limit;
      }
    }
  }

  // Inertia
  if (needToMove) {
    if (std::abs(cameraDirection->x) < speed * Math::Epsilon) {
      cameraDirection->x = 0.f;
    }

    if (std::abs(cameraDirection->y) < speed * Math::Epsilon) {
      cameraDirection->y = 0.f;
    }

    if (std::abs(cameraDirection->z) < speed * Math::Epsilon) {
      cameraDirection->z = 0.f;
    }
    cameraDirection->scaleInPlace(inertia);
  }
  if (needToRotate) {
    if (std::abs(cameraRotation->x) < speed * Math::Epsilon) {
      cameraRotation->x = 0.f;
    }

    if (std::abs(cameraRotation->y) < speed * Math::Epsilon) {
      cameraRotation->y = 0.f;
    }
    cameraRotation->scaleInPlace(inertia);
  }

  Camera::_checkInputs();
}

void TargetCamera::_updateCameraRotationMatrix()
{
  if (rotationQuaternion) {
    rotationQuaternion->toRotationMatrix(_cameraRotationMatrix);
  }
  else {
    Matrix::RotationYawPitchRollToRef(_rotation->y, _rotation->x, _rotation->z,
                                      _cameraRotationMatrix);
  }
}

TargetCamera& TargetCamera::_rotateUpVectorWithCameraRotationMatrix()
{
  Vector3::TransformNormalToRef(_defaultUp, _cameraRotationMatrix, upVector);
  return *this;
}

Matrix TargetCamera::_getViewMatrix()
{
  if (lockedTarget) {
    if (auto lockedTargetPosition = _getLockedTargetPosition()) {
      setTarget(*lockedTargetPosition);
    }
  }

  // Compute
  _updateCameraRotationMatrix();

  // Apply the changed rotation to the upVector
  if (rotationQuaternion
      && !stl_util::almost_equal(_cachedQuaternionRotationZ, rotationQuaternion->z)) {
    _rotateUpVectorWithCameraRotationMatrix();
    _cachedQuaternionRotationZ = rotationQuaternion->z;
  }
  else if (!stl_util::almost_equal(_cachedRotationZ, _rotation->z)) {
    _rotateUpVectorWithCameraRotationMatrix();
    _cachedRotationZ = _rotation->z;
  }

  Vector3::TransformCoordinatesToRef(*_referencePoint, _cameraRotationMatrix,
                                     _transformedReferencePoint);

  // Computing target and final matrix
  position().addToRef(_transformedReferencePoint, _currentTarget);

  if (updateUpVectorFromRotation) {
    if (rotationQuaternion) {
      auto yAxis = Axis::Y();
      yAxis.rotateByQuaternionToRef(*rotationQuaternion, upVector);
    }
    else {
      Quaternion::FromEulerVectorToRef(*_rotation, _tmpQuaternion);
      auto yAxis = Axis::Y();
      yAxis.rotateByQuaternionToRef(_tmpQuaternion, upVector);
    }
  }
  _computeViewMatrix(position, _currentTarget, upVector);

  return _viewMatrix;
}

void TargetCamera::_computeViewMatrix(const Vector3& iPosition, const Vector3& target,
                                      const Vector3& up)
{
  if (parent()) {
    auto parentWorldMatrix = parent()->getWorldMatrix();
    Vector3::TransformCoordinatesToRef(iPosition, parentWorldMatrix, _globalPosition);
    Vector3::TransformCoordinatesToRef(target, parentWorldMatrix, _globalCurrentTarget);
    Vector3::TransformNormalToRef(up, parentWorldMatrix, _globalCurrentUpVector);
    _markSyncedWithParent();
  }
  else {
    _globalPosition.copyFrom(iPosition);
    _globalCurrentTarget.copyFrom(target);
    _globalCurrentUpVector.copyFrom(up);
  }

  if (getScene()->useRightHandedSystem()) {
    Matrix::LookAtRHToRef(_globalPosition, _globalCurrentTarget, _globalCurrentUpVector,
                          _viewMatrix);
  }
  else {
    Matrix::LookAtLHToRef(_globalPosition, _globalCurrentTarget, _globalCurrentUpVector,
                          _viewMatrix);
  }
}

/** Camera rigs section **/
CameraPtr TargetCamera::createRigCamera(const std::string& iName, int /*cameraIndex*/)
{
  if (cameraRigMode != Camera::RIG_MODE_NONE) {
    auto rigCamera = TargetCamera::New(iName, position, getScene());
    if (cameraRigMode == Camera::RIG_MODE_VR || cameraRigMode == Camera::RIG_MODE_WEBVR) {
      if (!rotationQuaternion) {
        rotationQuaternion = std::make_unique<Quaternion>();
      }
      rigCamera->_cameraRigParams   = {};
      rigCamera->rotationQuaternion = std::make_unique<Quaternion>();
    }
    return rigCamera;
  }

  return nullptr;
}

void TargetCamera::_updateRigCameras()
{
  auto camLeft  = std::static_pointer_cast<TargetCamera>(_rigCameras[0]);
  auto camRight = std::static_pointer_cast<TargetCamera>(_rigCameras[1]);

  computeWorldMatrix();

  switch (cameraRigMode) {
    case Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH:
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL:
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED:
    case Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER: {
      // provisionnaly using _cameraRigParams.stereoHalfAngle instead of
      // calculations based on _cameraRigParams.interaxialDistance:
      auto leftSign
        = (cameraRigMode == Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED) ? 1.f : -1.f;
      auto rightSign
        = (cameraRigMode == Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED) ? -1.f : 1.f;
      _getRigCamPositionAndTarget(_cameraRigParams.stereoHalfAngle * leftSign, *camLeft);
      _getRigCamPositionAndTarget(_cameraRigParams.stereoHalfAngle * rightSign, *camRight);
    } break;
    case Camera::RIG_MODE_VR: {
      if (camLeft->rotationQuaternion) {
        camLeft->rotationQuaternion->copyFrom(*rotationQuaternion);
        camRight->rotationQuaternion->copyFrom(*rotationQuaternion);
      }
      else {
        camLeft->rotation().copyFrom(*_rotation);
        camRight->rotation().copyFrom(*_rotation);
      }
      camLeft->position().copyFrom(position);
      camRight->position().copyFrom(position);
    } break;
    default:
      break;
  }
  Camera::_updateRigCameras();
}

void TargetCamera::_getRigCamPositionAndTarget(float halfSpace, TargetCamera& rigCamera)
{
  const auto target = getTarget();
  target.subtractToRef(position, TargetCamera::_TargetFocalPoint);

  TargetCamera::_TargetFocalPoint.normalize().scaleInPlace(_initialFocalDistance);
  auto newFocalTarget = TargetCamera::_TargetFocalPoint.addInPlace(position);

  Matrix::TranslationToRef(-newFocalTarget.x, -newFocalTarget.y, -newFocalTarget.z,
                           TargetCamera::_TargetTransformMatrix);
  TargetCamera::_TargetTransformMatrix.multiplyToRef(Matrix::RotationY(halfSpace),
                                                     TargetCamera::_RigCamTransformMatrix);
  Matrix::TranslationToRef(newFocalTarget.x, newFocalTarget.y, newFocalTarget.z,
                           TargetCamera::_TargetTransformMatrix);

  TargetCamera::_RigCamTransformMatrix.multiplyToRef(TargetCamera::_TargetTransformMatrix,
                                                     TargetCamera::_RigCamTransformMatrix);

  Vector3::TransformCoordinatesToRef(position, TargetCamera::_RigCamTransformMatrix,
                                     rigCamera.position);
  rigCamera.setTarget(newFocalTarget);
}

std::string TargetCamera::getClassName() const
{
  return "TargetCamera";
}

json TargetCamera::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
