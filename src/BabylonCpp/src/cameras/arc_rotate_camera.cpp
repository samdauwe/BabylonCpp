#include <babylon/cameras/arc_rotate_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/cameras/auto_rotation_behavior.h>
#include <babylon/behaviors/cameras/bouncing_behavior.h>
#include <babylon/behaviors/cameras/framing_behavior.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

ArcRotateCamera::ArcRotateCamera(const string_t& iName, float iAlpha,
                                 float iBeta, float iRadius,
                                 const Nullable<Vector3>& iTarget, Scene* scene,
                                 bool setActiveOnSceneIfNoneActive)
    : TargetCamera{iName, Vector3::Zero(), scene, setActiveOnSceneIfNoneActive}
    , alpha{iAlpha}
    , beta{iBeta}
    , radius{iRadius}
    , target{this, &ArcRotateCamera::get_target, &ArcRotateCamera::set_target}
    , inertialAlphaOffset{0.f}
    , inertialBetaOffset{0.f}
    , inertialRadiusOffset{0.f}
    , lowerAlphaLimit{0.f}
    , upperAlphaLimit{0.f}
    , lowerBetaLimit{0.01f}
    , upperBetaLimit{Math::PI}
    , lowerRadiusLimit{0.f}
    , upperRadiusLimit{0.f}
    , inertialPanningX{0.f}
    , inertialPanningY{0.f}
    , pinchToPanMaxDistance{20.f}
    , panningOriginTarget{Vector3::Zero()}
    , panningInertia{0.9f}
    , zoomOnFactor{1.f}
    , targetScreenOffset{Vector2::Zero()}
    , allowUpsideDown{true}
    , panningAxis{::std::make_unique<Vector3>(1.f, 1.f, 0.f)}
    , bouncingBehavior{this, &ArcRotateCamera::get_bouncingBehavior}
    , useBouncingBehavior{this, &ArcRotateCamera::get_useBouncingBehavior,
                          &ArcRotateCamera::set_useBouncingBehavior}
    , framingBehavior{this, &ArcRotateCamera::get_framingBehavior}
    , useFramingBehavior{this, &ArcRotateCamera::get_useFramingBehavior,
                         &ArcRotateCamera::set_useFramingBehavior}
    , autoRotationBehavior{this, &ArcRotateCamera::get_autoRotationBehavior}
    , useAutoRotationBehavior{this,
                              &ArcRotateCamera::get_useAutoRotationBehavior,
                              &ArcRotateCamera::set_useAutoRotationBehavior}
    , checkCollisions{false}
    , collisionRadius{::std::make_unique<Vector3>(0.5f, 0.5f, 0.5f)}
    , _target{Vector3::Zero()}
    , _targetHost{nullptr}
    , _collider{nullptr}
    , _previousPosition{Vector3::Zero()}
    , _collisionVelocity{Vector3::Zero()}
    , _newPosition{Vector3::Zero()}
    , _collisionTriggered{false}
    , _bouncingBehavior{nullptr}
    , _framingBehavior{nullptr}
    , _autoRotationBehavior{nullptr}
    , _computationVector{Vector3::Zero()}
{
  if (iTarget) {
    setTarget(*iTarget);
  }
  getViewMatrix();
  inputs = ::std::make_unique<ArcRotateCameraInputsManager>(this);
  inputs->addKeyboard().addMouseWheel().addPointers();
}

ArcRotateCamera::~ArcRotateCamera()
{
}

IReflect::Type ArcRotateCamera::type() const
{
  return IReflect::Type::ARCROTATECAMERA;
}

void ArcRotateCamera::_initCache()
{
  TargetCamera::_initCache();

  _cache._target
    = Vector3(numeric_limits_t<float>::max(), numeric_limits_t<float>::max(),
              numeric_limits_t<float>::max());
  _cache.alpha              = 0.f;
  _cache.beta               = 0.f;
  _cache.radius             = 0.f;
  _cache.targetScreenOffset = Vector2::Zero();
}

void ArcRotateCamera::_updateCache(bool ignoreParentClass)
{
  if (!ignoreParentClass) {
    TargetCamera::_updateCache(ignoreParentClass);
  }

  _cache._target.copyFrom(_getTargetPosition());
  _cache.alpha  = alpha;
  _cache.beta   = beta;
  _cache.radius = radius;
  _cache.targetScreenOffset.copyFrom(targetScreenOffset);
}

Camera& ArcRotateCamera::storeState()
{
  _storedAlpha  = alpha;
  _storedBeta   = beta;
  _storedRadius = radius;
  _storedTarget = _getTargetPosition().copy();

  return Camera::storeState();
}

bool ArcRotateCamera::_restoreStateValues()
{
  if (!Camera::_restoreStateValues()) {
    return false;
  }

  alpha  = _storedAlpha;
  beta   = _storedBeta;
  radius = _storedRadius;
  setTarget(_storedTarget.copy());

  inertialAlphaOffset  = 0.f;
  inertialBetaOffset   = 0.f;
  inertialRadiusOffset = 0.f;
  inertialPanningX     = 0.f;
  inertialPanningY     = 0.f;

  return true;
}

Vector3 ArcRotateCamera::_getTargetPosition()
{
  if (_targetHost) {
    auto pos = _targetHost->getAbsolutePosition();
    if (_targetBoundingCenter) {
      pos.addToRef(*_targetBoundingCenter, _target);
    }
    else {
      _target.copyFrom(pos);
    }
  }

  auto lockedTargetPosition = _getLockedTargetPosition();

  if (lockedTargetPosition) {
    return *lockedTargetPosition;
  }

  return _target;
}

bool ArcRotateCamera::_isSynchronizedViewMatrix()
{
  if (!TargetCamera::_isSynchronizedViewMatrix()) {
    return false;
  }

  return _cache._target.equals(_getTargetPosition())
         && stl_util::almost_equal(_cache.alpha, alpha)
         && stl_util::almost_equal(_cache.beta, beta)
         && stl_util::almost_equal(_cache.radius, radius)
         && _cache.targetScreenOffset.equals(targetScreenOffset);
}

void ArcRotateCamera::attachControl(ICanvas* canvas, bool noPreventDefault,
                                    bool useCtrlForPanning,
                                    MouseButtonType panningMouseButton)
{
  _useCtrlForPanning  = useCtrlForPanning;
  _panningMouseButton = panningMouseButton;

  inputs->attachElement(canvas, noPreventDefault);

  _reset = [&]() {
    inertialAlphaOffset  = 0.f;
    inertialBetaOffset   = 0.f;
    inertialRadiusOffset = 0.f;
    inertialPanningX     = 0.f;
    inertialPanningY     = 0.f;
  };
}

void ArcRotateCamera::detachControl(ICanvas* canvas)
{
  inputs->detachElement(canvas);

  if (_reset) {
    _reset();
  }
}

void ArcRotateCamera::_checkInputs()
{
  // if (async) collision inspection was triggered, don't update the camera's
  // position - until the collision callback was called.
  if (_collisionTriggered) {
    return;
  }

  inputs->checkInputs();

  // Inertia
  if (!stl_util::almost_equal(inertialAlphaOffset, 0.f)
      || !stl_util::almost_equal(inertialBetaOffset, 0.f)
      || !stl_util::almost_equal(inertialRadiusOffset, 0.f)) {

    if (beta <= 0.f) {
      inertialAlphaOffset *= -1.f;
    }
    if (getScene()->useRightHandedSystem()) {
      inertialAlphaOffset *= -1.f;
    }
    if (parent && parent()->_getWorldMatrixDeterminant() < 0.f) {
      inertialAlphaOffset *= -1.f;
    }

    alpha += inertialAlphaOffset;
    beta += inertialBetaOffset;

    radius -= inertialRadiusOffset;
    inertialAlphaOffset *= inertia;
    inertialBetaOffset *= inertia;
    inertialRadiusOffset *= inertia;
    if (::std::abs(inertialAlphaOffset) < Math::Epsilon) {
      inertialAlphaOffset = 0.f;
    }
    if (::std::abs(inertialBetaOffset) < Math::Epsilon) {
      inertialBetaOffset = 0.f;
    }
    if (::std::abs(inertialRadiusOffset) < speed * Math::Epsilon) {
      inertialRadiusOffset = 0.f;
    }
  }

  // Panning inertia
  if (!stl_util::almost_equal(inertialPanningX, 0.f)
      || !stl_util::almost_equal(inertialPanningY, 0.f)) {
    if (!_localDirection) {
      _localDirection       = ::std::make_unique<Vector3>(Vector3::Zero());
      _transformedDirection = Vector3::Zero();
    }

    _localDirection->copyFromFloats(inertialPanningX, inertialPanningY,
                                    inertialPanningY);
    _localDirection->multiplyInPlace(*panningAxis);
    _viewMatrix.invertToRef(_cameraTransformMatrix);
    Vector3::TransformNormalToRef(*_localDirection, _cameraTransformMatrix,
                                  _transformedDirection);
    // Eliminate y if map panning is enabled (panningAxis == 1,0,1)
    if (!stl_util::almost_equal(panningAxis->y, 0.f)) {
      _transformedDirection.y = 0;
    }

    if (!_targetHost) {
      if (panningDistanceLimit) {
        _transformedDirection.addInPlace(_target);
        auto distanceSquared = Vector3::DistanceSquared(_transformedDirection,
                                                        panningOriginTarget);
        if (distanceSquared <= (panningDistanceLimit * panningDistanceLimit)) {
          _target.copyFrom(_transformedDirection);
        }
      }
      else {
        _target.addInPlace(_transformedDirection);
      }
    }

    inertialPanningX *= panningInertia;
    inertialPanningY *= panningInertia;

    if (::std::abs(inertialPanningX) < speed * Math::Epsilon) {
      inertialPanningX = 0.f;
    }
    if (::std::abs(inertialPanningY) < speed * Math::Epsilon) {
      inertialPanningY = 0.f;
    }
  }

  // Limits
  _checkLimits();

  TargetCamera::_checkInputs();
}

void ArcRotateCamera::_checkLimits()
{
  if (lowerBetaLimit == 0.f) {
    if (allowUpsideDown && beta > Math::PI) {
      beta = beta - (2 * Math::PI);
    }
  }
  else {
    if (beta < lowerBetaLimit) {
      beta = lowerBetaLimit;
    }
  }

  if (upperBetaLimit == 0.f) {
    if (allowUpsideDown && beta < -Math::PI) {
      beta = beta + (2 * Math::PI);
    }
  }
  else {
    if (beta > upperBetaLimit) {
      beta = upperBetaLimit;
    }
  }

  if ((!stl_util::almost_equal(lowerAlphaLimit, 0.f))
      && alpha < lowerAlphaLimit) {
    alpha = lowerAlphaLimit;
  }
  if ((!stl_util::almost_equal(upperAlphaLimit, 0.f))
      && alpha > upperAlphaLimit) {
    alpha = upperAlphaLimit;
  }

  if ((!stl_util::almost_equal(lowerRadiusLimit, 0.f))
      && radius < lowerRadiusLimit) {
    radius = lowerRadiusLimit;
  }
  if ((!stl_util::almost_equal(upperRadiusLimit, 0.f))
      && radius > upperRadiusLimit) {
    radius = upperRadiusLimit;
  }
}

void ArcRotateCamera::rebuildAnglesAndRadius()
{
  position.subtractToRef(_getTargetPosition(), _computationVector);
  radius = _computationVector.length();

  if (radius == 0.f) {
    radius = 0.0001f; // Just to avoid division by zero
  }

  // Alpha
  alpha = ::std::acos(_computationVector.x
                      / ::std::sqrt(::std::pow(_computationVector.x, 2.f)
                                    + ::std::pow(_computationVector.z, 2.f)));

  if (_computationVector.z < 0.f) {
    alpha = Math::PI2 - alpha;
  }

  // Beta
  beta = ::std::acos(_computationVector.y / radius);

  _checkLimits();
}

void ArcRotateCamera::setPosition(const Vector3& iPosition)
{
  if (position.equals(iPosition)) {
    return;
  }

  position.copyFrom(iPosition);

  rebuildAnglesAndRadius();
}

Vector3& ArcRotateCamera::get_target()
{
  return _target;
}

void ArcRotateCamera::set_target(const Vector3& value)
{
  setTarget(value);
}

void ArcRotateCamera::setTarget(AbstractMesh* iTarget, bool toBoundingCenter,
                                bool /*allowSamePosition*/)
{
  {
    if (toBoundingCenter) {
      _targetBoundingCenter
        = iTarget->getBoundingInfo().boundingBox.centerWorld.clone();
    }
    else {
      _targetBoundingCenter.reset(nullptr);
    }
    _targetHost = iTarget;
    _target     = _getTargetPosition();
  }

  rebuildAnglesAndRadius();
}

void ArcRotateCamera::setTarget(const Vector3& iTarget,
                                bool /*toBoundingCenter*/,
                                bool allowSamePosition)
{
  Vector3 newTarget{iTarget};
  if (!allowSamePosition && _getTargetPosition().equals(newTarget)) {
    return;
  }
  _target = newTarget;
  _targetBoundingCenter.reset(nullptr);

  rebuildAnglesAndRadius();
}

unique_ptr_t<BouncingBehavior>& ArcRotateCamera::get_bouncingBehavior()
{
  return _bouncingBehavior;
}

bool ArcRotateCamera::get_useBouncingBehavior() const
{
  return _bouncingBehavior != nullptr;
}

void ArcRotateCamera::set_useBouncingBehavior(bool value)
{
  if (value == useBouncingBehavior()) {
    return;
  }

  if (value) {
    _bouncingBehavior = ::std::make_unique<BouncingBehavior>();
    addBehavior(dynamic_cast<Behavior<Node>*>(_bouncingBehavior.get()));
  }
  else if (_bouncingBehavior) {
    removeBehavior(dynamic_cast<Behavior<Node>*>(_bouncingBehavior.get()));
    _bouncingBehavior = nullptr;
  }
}

unique_ptr_t<FramingBehavior>& ArcRotateCamera::get_framingBehavior()
{
  return _framingBehavior;
}

bool ArcRotateCamera::get_useFramingBehavior() const
{
  return _framingBehavior != nullptr;
}

void ArcRotateCamera::set_useFramingBehavior(bool value)
{
  if (value == useFramingBehavior()) {
    return;
  }

  if (value) {
    _framingBehavior = ::std::make_unique<FramingBehavior>();
    addBehavior(dynamic_cast<Behavior<Node>*>(_framingBehavior.get()));
  }
  else if (_framingBehavior) {
    removeBehavior(dynamic_cast<Behavior<Node>*>(_framingBehavior.get()));
    _framingBehavior = nullptr;
  }
}

unique_ptr_t<AutoRotationBehavior>& ArcRotateCamera::get_autoRotationBehavior()
{
  return _autoRotationBehavior;
}

bool ArcRotateCamera::get_useAutoRotationBehavior() const
{
  return _autoRotationBehavior != nullptr;
}

void ArcRotateCamera::set_useAutoRotationBehavior(bool value)
{
  if (value == useAutoRotationBehavior()) {
    return;
  }

  if (value) {
    _autoRotationBehavior = ::std::make_unique<AutoRotationBehavior>();
    addBehavior(dynamic_cast<Behavior<Node>*>(_autoRotationBehavior.get()));
  }
  else if (_autoRotationBehavior) {
    removeBehavior(dynamic_cast<Behavior<Node>*>(_autoRotationBehavior.get()));
    _autoRotationBehavior = nullptr;
  }
}

Matrix ArcRotateCamera::_getViewMatrix()
{
  // Compute
  const float cosa = ::std::cos(alpha);
  const float sina = ::std::sin(alpha);
  const float cosb = ::std::cos(beta);
  float sinb       = ::std::sin(beta);

  if (sinb == 0.f) {
    sinb = 0.0001f;
  }

  auto _target = _getTargetPosition();
  _computationVector.copyFromFloats(radius * cosa * sinb, radius * cosb,
                                    radius * sina * sinb);
  _target.addToRef(_computationVector, _newPosition);
  if (getScene()->collisionsEnabled && checkCollisions) {
    if (!_collider) {
      _collider = ::std::make_unique<Collider>();
    }
    _collider->_radius = *collisionRadius;
    _newPosition.subtractToRef(position, _collisionVelocity);
    _collisionTriggered = true;
    getScene()->collisionCoordinator->getNewPosition(
      position, _collisionVelocity, _collider.get(), 3, nullptr,
      [&](int collisionId, Vector3& newPosition, AbstractMesh* collidedMesh) {
        _onCollisionPositionChange(collisionId, newPosition, collidedMesh);
      },
      static_cast<unsigned>(uniqueId));
  }
  else {
    position.copyFrom(_newPosition);

    auto up = upVector;
    if (allowUpsideDown && sinb < 0.f) {
      up = up.negate();
    }

    _computeViewMatrix(position, target, up);

    _viewMatrix.m[12] += targetScreenOffset.x;
    _viewMatrix.m[13] += targetScreenOffset.y;
  }
  _currentTarget = _target;

  return _viewMatrix;
}

void ArcRotateCamera::_onCollisionPositionChange(int /*collisionId*/,
                                                 Vector3& newPosition,
                                                 AbstractMesh* collidedMesh)
{
  if (getScene()->workerCollisions() && checkCollisions) {
    newPosition.multiplyInPlace(_collider->_radius);
  }

  if (!collidedMesh) {
    _previousPosition.copyFrom(position);
  }
  else {
    setPosition(newPosition);

    if (onCollide) {
      onCollide(collidedMesh);
    }
  }

  // Recompute because of constraints
  const float cosa = ::std::cos(alpha);
  const float sina = ::std::sin(alpha);
  const float cosb = ::std::cos(beta);
  float sinb       = ::std::sin(beta);

  if (sinb == 0.f) {
    sinb = 0.0001f;
  }

  auto _target = _getTargetPosition();
  _computationVector.copyFromFloats(radius * cosa * sinb, radius * cosb,
                                    radius * sina * sinb);
  _target.addToRef(_computationVector, _newPosition);
  position.copyFrom(_newPosition);

  auto up = upVector;
  if (allowUpsideDown && beta < 0.f) {
    up = up.negate();
  }

  Matrix::LookAtLHToRef(position, _target, up, _viewMatrix);
  _viewMatrix.m[12] += targetScreenOffset.x;
  _viewMatrix.m[13] += targetScreenOffset.y;

  _collisionTriggered = false;
}

void ArcRotateCamera::zoomOn(const vector_t<AbstractMesh*> meshes,
                             bool doNotUpdateMaxZ)
{
  auto _meshes = meshes.empty() ? getScene()->getMeshes() : meshes;

  auto minMaxVector = Mesh::GetMinMax(_meshes);
  auto distance     = Vector3::Distance(minMaxVector.min, minMaxVector.max);

  radius = distance * zoomOnFactor;

  focusOn({minMaxVector.min, minMaxVector.max, distance}, doNotUpdateMaxZ);
}

void ArcRotateCamera::focusOn(
  const MinMaxDistance& meshesOrMinMaxVectorAndDistance, bool doNotUpdateMaxZ)
{
  _target = Mesh::Center(meshesOrMinMaxVectorAndDistance);

  if (!doNotUpdateMaxZ) {
    maxZ = meshesOrMinMaxVectorAndDistance.distance * 2.f;
  }
}

Camera* ArcRotateCamera::createRigCamera(const string_t& iName, int cameraIndex)
{
  float alphaShift = 0.f;

  switch (cameraRigMode) {
    case Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH():
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL():
    case Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER():
    case Camera::RIG_MODE_VR():
      alphaShift
        = _cameraRigParams.stereoHalfAngle * (cameraIndex == 0 ? 1 : -1);
      break;
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED():
      alphaShift
        = _cameraRigParams.stereoHalfAngle * (cameraIndex == 0 ? -1 : 1);
      break;
    default:
      break;
  }

  auto rigCam = ArcRotateCamera::New(iName, alpha + alphaShift, beta, radius,
                                     _target, getScene());
  return rigCam;
}

void ArcRotateCamera::_updateRigCameras()
{
  auto camLeft  = dynamic_cast<ArcRotateCamera*>(_rigCameras[0]);
  auto camRight = dynamic_cast<ArcRotateCamera*>(_rigCameras[1]);

  camLeft->beta = camRight->beta = beta;
  camLeft->radius = camRight->radius = radius;

  switch (cameraRigMode) {
    case Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH():
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL():
    case Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER():
    case Camera::RIG_MODE_VR():
      camLeft->alpha  = alpha - _cameraRigParams.stereoHalfAngle;
      camRight->alpha = alpha + _cameraRigParams.stereoHalfAngle;
      break;
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED():
      camLeft->alpha  = alpha + _cameraRigParams.stereoHalfAngle;
      camRight->alpha = alpha - _cameraRigParams.stereoHalfAngle;
      break;
    default:
      break;
  }
  TargetCamera::_updateRigCameras();
}

void ArcRotateCamera::dispose(bool doNotRecurse,
                              bool disposeMaterialAndTextures)
{
  inputs->clear();
  TargetCamera::dispose(doNotRecurse, disposeMaterialAndTextures);
}

const string_t ArcRotateCamera::getClassName() const
{
  return "ArcRotateCamera";
}

Json::object ArcRotateCamera::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
