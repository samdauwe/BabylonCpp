#include <babylon/cameras/target_camera.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/core/time.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/math/tmp.h>

namespace BABYLON {

TargetCamera::TargetCamera(const std::string& iName, const Vector3& iPosition,
                           Scene* scene, bool setActiveOnSceneIfNoneActive)
    : Camera(iName, iPosition, scene, setActiveOnSceneIfNoneActive)
    , cameraDirection{std::make_unique<Vector3>(0.f, 0.f, 0.f)}
    , cameraRotation{std::make_unique<Vector2>(0.f, 0.f)}
    , rotation{std::make_unique<Vector3>(0.f, 0.f, 0.f)}
    , rotationQuaternion{nullptr}
    , speed{2.f}
    , noRotationConstraint{false}
    , lockedTarget{nullptr}
    , _currentTarget{Vector3::Zero()}
    , _viewMatrix{Matrix::Zero()}
    , _camMatrix{Matrix::Zero()}
    , _cameraTransformMatrix{Matrix::Zero()}
    , _cameraRotationMatrix{Matrix::Zero()}
    , _referencePoint{std::make_unique<Vector3>(0.f, 0.f, 1.f)}
    , _transformedReferencePoint{Vector3::Zero()}
    , _globalCurrentTarget{Vector3::Zero()}
    , _globalCurrentUpVector{Vector3::Zero()}
    , _defaultUp{Vector3::Up()}
    , _cachedRotationZ{0.f}
    , _cachedQuaternionRotationZ{0.f}
{
  _initCache();
}

TargetCamera::~TargetCamera()
{
}

Type TargetCamera::type() const
{
  return Type::TARGETCAMERA;
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
  _storedPosition = position.copy();
  _storedRotation = rotation->copy();
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
  rotation = std::make_unique<Vector3>(_storedRotation);

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
  _cache.lockedTarget = std::make_unique<Vector3>(
    std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
    std::numeric_limits<float>::max());
  _cache.rotation = Vector3(std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max(),
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

  _cache.rotation.copyFrom(*rotation);
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

  return (_cache.lockedTarget ?
            _cache.lockedTarget->equals(*lockedTargetPosition) :
            !lockedTargetPosition)
         && (rotationQuaternion ?
               rotationQuaternion->equals(_cache.rotationQuaternion) :
               _cache.rotation.equals(*rotation));
}

/** Methods **/
float TargetCamera::_computeLocalCameraSpeed()
{
  auto engine = getEngine();

  return speed
         * std::sqrt((engine->getDeltaTime() / (engine->getFps() * 100.f)));
}

void TargetCamera::setRotation(const Vector3& newRotation)
{
  rotation = std::make_unique<Vector3>(newRotation);
}

Vector3& TargetCamera::getRotation()
{
  return *rotation;
}

/** Target **/
void TargetCamera::setTarget(const Vector3& target)
{
  upVector.normalize();

  if (stl_util::almost_equal(position.z, target.z)) {
    position.z += Math::Epsilon;
  }

  Matrix::LookAtLHToRef(position, target, _defaultUp, _camMatrix);
  _camMatrix.invert();

  rotation->x = std::atan(_camMatrix.m[6] / _camMatrix.m[10]);

  auto vDir = target.subtract(position);

  if (vDir.x >= 0.f) {
    rotation->y = (-std::atan(vDir.z / vDir.x) + Math::PI / 2.f);
  }
  else {
    rotation->y = (-std::atan(vDir.z / vDir.x) - Math::PI / 2.f);
  }

  rotation->z = 0.f;

  if (isNan(rotation->x)) {
    rotation->x = 0.f;
  }

  if (isNan(rotation->y)) {
    rotation->y = 0.f;
  }

  if (isNan(rotation->z)) {
    rotation->z = 0.f;
  }

  if (rotationQuaternion) {
    Quaternion::RotationYawPitchRollToRef(rotation->y, rotation->x, rotation->z,
                                          *rotationQuaternion);
  }
}

Vector3& TargetCamera::getTarget()
{
  return _currentTarget;
}

bool TargetCamera::_decideIfNeedsToMove()
{
  return std::abs(cameraDirection->x) > 0.f
         || std::abs(cameraDirection->y) > 0.f
         || std::abs(cameraDirection->z) > 0.f;
}

void TargetCamera::_updatePosition()
{
  if (parent()) {
    parent()->getWorldMatrix().invertToRef(Tmp::MatrixArray[0]);
    Vector3::TransformNormalToRef(*cameraDirection, Tmp::MatrixArray[0],
                                  Tmp::Vector3Array[0]);
    position.addInPlace(Tmp::Vector3Array[0]);
    return;
  }
  position.addInPlace(*cameraDirection);
}

void TargetCamera::_checkInputs()
{
  bool needToMove = _decideIfNeedsToMove();
  bool needToRotate
    = std::abs(cameraRotation->x) > 0.f || std::abs(cameraRotation->y) > 0.f;

  // Move
  if (needToMove) {
    _updatePosition();
  }

  // Rotate
  if (needToRotate) {
    rotation->x += cameraRotation->x;
    rotation->y += cameraRotation->y;

    // Rotate, if quaternion is set and rotation was used
    if (rotationQuaternion) {
      float len = rotation->lengthSquared();
      if (len > 0) {
        Quaternion::RotationYawPitchRollToRef(rotation->y, rotation->x,
                                              rotation->z, *rotationQuaternion);
      }
    }

    if (!noRotationConstraint) {
      float limit = Math::PI_2 * 0.95f;

      if (rotation->x > limit) {
        rotation->x = limit;
      }
      if (rotation->x < -limit) {
        rotation->x = -limit;
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
    Matrix::RotationYawPitchRollToRef(rotation->y, rotation->x, rotation->z,
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
      && !stl_util::almost_equal(_cachedQuaternionRotationZ,
                                 rotationQuaternion->z)) {
    _rotateUpVectorWithCameraRotationMatrix();
    _cachedQuaternionRotationZ = rotationQuaternion->z;
  }
  else if (!stl_util::almost_equal(_cachedRotationZ, rotation->z)) {
    _rotateUpVectorWithCameraRotationMatrix();
    _cachedRotationZ = rotation->z;
  }

  Vector3::TransformCoordinatesToRef(*_referencePoint, _cameraRotationMatrix,
                                     _transformedReferencePoint);

  // Computing target and final matrix
  position.addToRef(_transformedReferencePoint, _currentTarget);

  _computeViewMatrix(position, _currentTarget, upVector);

  return _viewMatrix;
}

void TargetCamera::_computeViewMatrix(const Vector3& iPosition,
                                      const Vector3& target, const Vector3& up)
{
  if (parent()) {
    auto parentWorldMatrix = parent()->getWorldMatrix();
    Vector3::TransformCoordinatesToRef(iPosition, parentWorldMatrix,
                                       _globalPosition);
    Vector3::TransformCoordinatesToRef(target, parentWorldMatrix,
                                       _globalCurrentTarget);
    Vector3::TransformNormalToRef(up, parentWorldMatrix,
                                  _globalCurrentUpVector);
    _markSyncedWithParent();
  }
  else {
    _globalPosition.copyFrom(iPosition);
    _globalCurrentTarget.copyFrom(target);
    _globalCurrentUpVector.copyFrom(up);
  }

  if (getScene()->useRightHandedSystem()) {
    Matrix::LookAtRHToRef(_globalPosition, _globalCurrentTarget,
                          _globalCurrentUpVector, _viewMatrix);
  }
  else {
    Matrix::LookAtLHToRef(_globalPosition, _globalCurrentTarget,
                          _globalCurrentUpVector, _viewMatrix);
  }
}

/** Camera rigs section **/
CameraPtr TargetCamera::createRigCamera(const std::string& iName,
                                        int /*cameraIndex*/)
{
  if (cameraRigMode != Camera::RIG_MODE_NONE) {
    auto rigCamera = TargetCamera::New(iName, position, getScene());
    if (cameraRigMode == Camera::RIG_MODE_VR
        || cameraRigMode == Camera::RIG_MODE_WEBVR) {
      if (!rotationQuaternion) {
        rotationQuaternion = std::make_unique<Quaternion>();
      }
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

  switch (cameraRigMode) {
    case Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH:
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL:
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED:
    case Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER: {
      // provisionnaly using _cameraRigParams.stereoHalfAngle instead of
      // calculations based on _cameraRigParams.interaxialDistance:
      float leftSign = (cameraRigMode
                        == Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED) ?
                         1.f :
                         -1.f;
      float rightSign
        = (cameraRigMode
           == Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED) ?
            -1.f :
            1.f;
      _getRigCamPosition(_cameraRigParams.stereoHalfAngle * leftSign,
                         camLeft->position);
      _getRigCamPosition(_cameraRigParams.stereoHalfAngle * rightSign,
                         camRight->position);

      camLeft->setTarget(getTarget());
      camRight->setTarget(getTarget());
    } break;
    case Camera::RIG_MODE_VR: {
      if (camLeft->rotationQuaternion) {
        camLeft->rotationQuaternion->copyFrom(*rotationQuaternion);
        camRight->rotationQuaternion->copyFrom(*rotationQuaternion);
      }
      else {
        camLeft->rotation->copyFrom(*rotation);
        camRight->rotation->copyFrom(*rotation);
      }
      camLeft->position.copyFrom(position);
      camRight->position.copyFrom(position);
    } break;
    default:
      break;
  }
  Camera::_updateRigCameras();
}

void TargetCamera::_getRigCamPosition(float halfSpace, Vector3& result)
{
  const Vector3& target = getTarget();
  Matrix::Translation(-target.x, -target.y, -target.z)
    .multiplyToRef(Matrix::RotationY(halfSpace), _rigCamTransformMatrix);

  _rigCamTransformMatrix = _rigCamTransformMatrix.multiply(
    Matrix::Translation(target.x, target.y, target.z));

  Vector3::TransformCoordinatesToRef(position, _rigCamTransformMatrix, result);
}

const std::string TargetCamera::getClassName() const
{
  return "TargetCamera";
}

json TargetCamera::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
