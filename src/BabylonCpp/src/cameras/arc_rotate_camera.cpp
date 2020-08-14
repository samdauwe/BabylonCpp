#include <babylon/cameras/arc_rotate_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/cameras/auto_rotation_behavior.h>
#include <babylon/behaviors/cameras/bouncing_behavior.h>
#include <babylon/behaviors/cameras/framing_behavior.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

bool ArcRotateCamera::NodeConstructorAdded = false;

void ArcRotateCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor("ArcRotateCamera", [](const std::string& iName, Scene* scene,
                                                 const std::optional<json>& /*options*/) {
    return ArcRotateCamera::New(iName, 0.f, 0.f, 1.f, Vector3::Zero(), scene);
  });
  ArcRotateCamera::NodeConstructorAdded = true;
}

ArcRotateCamera::ArcRotateCamera(const std::string& iName, float iAlpha, float iBeta, float iRadius,
                                 const std::optional<Vector3>& iTarget, Scene* scene,
                                 bool setActiveOnSceneIfNoneActive)
    : TargetCamera{iName, Vector3::Zero(), scene, setActiveOnSceneIfNoneActive}
    , alpha{0.f}
    , beta{0.f}
    , radius{0.f}
    , target{this, &ArcRotateCamera::get_target, &ArcRotateCamera::set_target}
    , upVector{this, &ArcRotateCamera::get_upVector, &ArcRotateCamera::set_upVector}
    , inertialAlphaOffset{0.f}
    , inertialBetaOffset{0.f}
    , inertialRadiusOffset{0.f}
    , lowerAlphaLimit{std::nullopt}
    , upperAlphaLimit{std::nullopt}
    , lowerBetaLimit{0.01f}
    , upperBetaLimit{Math::PI - 0.01f}
    , lowerRadiusLimit{std::nullopt}
    , upperRadiusLimit{std::nullopt}
    , inertialPanningX{0.f}
    , inertialPanningY{0.f}
    , pinchToPanMaxDistance{20.f}
    , panningDistanceLimit{std::nullopt}
    , panningOriginTarget{Vector3::Zero()}
    , panningInertia{0.9f}
    , zoomOnFactor{1.f}
    , targetScreenOffset{Vector2::Zero()}
    , allowUpsideDown{true}
    , useInputToRestoreState{true}
    , panningAxis{std::make_unique<Vector3>(1.f, 1.f, 0.f)}
    , bouncingBehavior{this, &ArcRotateCamera::get_bouncingBehavior}
    , useBouncingBehavior{this, &ArcRotateCamera::get_useBouncingBehavior,
                          &ArcRotateCamera::set_useBouncingBehavior}
    , framingBehavior{this, &ArcRotateCamera::get_framingBehavior}
    , useFramingBehavior{this, &ArcRotateCamera::get_useFramingBehavior,
                         &ArcRotateCamera::set_useFramingBehavior}
    , autoRotationBehavior{this, &ArcRotateCamera::get_autoRotationBehavior}
    , useAutoRotationBehavior{this, &ArcRotateCamera::get_useAutoRotationBehavior,
                              &ArcRotateCamera::set_useAutoRotationBehavior}
    , checkCollisions{false}
    , collisionRadius{std::make_unique<Vector3>(0.5f, 0.5f, 0.5f)}
    , _targetHost{nullptr}
    , _upVector{Vector3::Up()}
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
  _target = Vector3::Zero();
  if (iTarget.has_value()) {
    setTarget(*iTarget);
  }

  alpha  = iAlpha;
  beta   = iBeta;
  radius = iRadius;

  getViewMatrix();
  inputs = std::make_unique<ArcRotateCameraInputsManager>(this);
  inputs->addKeyboard().addMouseWheel().addPointers();
}

ArcRotateCamera::~ArcRotateCamera() = default;

Type ArcRotateCamera::type() const
{
  return Type::ARCROTATECAMERA;
}

Vector3& ArcRotateCamera::get_target()
{
  return _target;
}

void ArcRotateCamera::set_target(const Vector3& value)
{
  setTarget(value);
}

Vector3& ArcRotateCamera::get_upVector()
{
  return _upVector;
}

void ArcRotateCamera::set_upVector(const Vector3& vec)
{
  auto _vec = vec;
  _vec.normalize();
  _upVector.copyFrom(_vec);
  setMatUp();
}

void ArcRotateCamera::setMatUp()
{
  // from y-up to custom-up (used in _getViewMatrix)
  Matrix::RotationAlignToRef(Vector3::UpReadOnly(), _upVector, _YToUpMatrix);

  // from custom-up to y-up (used in rebuildAnglesAndRadius)
  Matrix::RotationAlignToRef(_upVector, Vector3::UpReadOnly(), _upToYMatrix);
}

std::unique_ptr<BouncingBehavior>& ArcRotateCamera::get_bouncingBehavior()
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
    _bouncingBehavior = std::make_unique<BouncingBehavior>();
    addBehavior(dynamic_cast<Behavior<Node>*>(_bouncingBehavior.get()));
  }
  else if (_bouncingBehavior) {
    removeBehavior(dynamic_cast<Behavior<Node>*>(_bouncingBehavior.get()));
    _bouncingBehavior = nullptr;
  }
}

std::unique_ptr<FramingBehavior>& ArcRotateCamera::get_framingBehavior()
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
    _framingBehavior = std::make_unique<FramingBehavior>();
    addBehavior(dynamic_cast<Behavior<Node>*>(_framingBehavior.get()));
  }
  else if (_framingBehavior) {
    removeBehavior(dynamic_cast<Behavior<Node>*>(_framingBehavior.get()));
    _framingBehavior = nullptr;
  }
}

std::unique_ptr<AutoRotationBehavior>& ArcRotateCamera::get_autoRotationBehavior()
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
    _autoRotationBehavior = std::make_unique<AutoRotationBehavior>();
    addBehavior(dynamic_cast<Behavior<Node>*>(_autoRotationBehavior.get()));
  }
  else if (_autoRotationBehavior) {
    removeBehavior(dynamic_cast<Behavior<Node>*>(_autoRotationBehavior.get()));
    _autoRotationBehavior = nullptr;
  }
}

void ArcRotateCamera::_initCache()
{
  TargetCamera::_initCache();

  _cache._target = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max());
  _cache.alpha   = 0.f;
  _cache.beta    = 0.f;
  _cache.radius  = 0.f;
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

Vector3 ArcRotateCamera::_getTargetPosition()
{
  if (_targetHost) {
    auto pos = _targetHost->absolutePosition();
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

Camera& ArcRotateCamera::storeState()
{
  _storedAlpha              = alpha;
  _storedBeta               = beta;
  _storedRadius             = radius;
  _storedTarget             = _getTargetPosition().copy();
  _storedTargetScreenOffset = targetScreenOffset.copy();

  return Camera::storeState();
}

bool ArcRotateCamera::_restoreStateValues()
{
  if (!Camera::_restoreStateValues()) {
    return false;
  }

  setTarget(_storedTarget.copy());
  alpha              = _storedAlpha;
  beta               = _storedBeta;
  radius             = _storedRadius;
  targetScreenOffset = _storedTargetScreenOffset.copy();

  inertialAlphaOffset  = 0.f;
  inertialBetaOffset   = 0.f;
  inertialRadiusOffset = 0.f;
  inertialPanningX     = 0.f;
  inertialPanningY     = 0.f;

  return true;
}

bool ArcRotateCamera::_isSynchronizedViewMatrix()
{
  if (!TargetCamera::_isSynchronizedViewMatrix()) {
    return false;
  }

  return _cache._target.equals(_getTargetPosition()) && stl_util::almost_equal(_cache.alpha, alpha)
         && stl_util::almost_equal(_cache.beta, beta)
         && stl_util::almost_equal(_cache.radius, radius)
         && _cache.targetScreenOffset.equals(targetScreenOffset);
}

void ArcRotateCamera::attachControl(ICanvas* canvas, bool noPreventDefault, bool useCtrlForPanning,
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
    if (std::abs(inertialAlphaOffset) < Math::Epsilon) {
      inertialAlphaOffset = 0.f;
    }
    if (std::abs(inertialBetaOffset) < Math::Epsilon) {
      inertialBetaOffset = 0.f;
    }
    if (std::abs(inertialRadiusOffset) < speed * Math::Epsilon) {
      inertialRadiusOffset = 0.f;
    }
  }

  // Panning inertia
  if (!stl_util::almost_equal(inertialPanningX, 0.f)
      || !stl_util::almost_equal(inertialPanningY, 0.f)) {
    if (!_localDirection) {
      _localDirection       = std::make_unique<Vector3>(Vector3::Zero());
      _transformedDirection = Vector3::Zero();
    }

    _localDirection->copyFromFloats(inertialPanningX, inertialPanningY, inertialPanningY);
    _localDirection->multiplyInPlace(*panningAxis);
    _viewMatrix.invertToRef(_cameraTransformMatrix);
    Vector3::TransformNormalToRef(*_localDirection, _cameraTransformMatrix, _transformedDirection);
    // Eliminate y if map panning is enabled (panningAxis == 1,0,1)
    if (!stl_util::almost_equal(panningAxis->y, 0.f)) {
      _transformedDirection.y = 0;
    }

    if (!_targetHost) {
      if (panningDistanceLimit.has_value()) {
        _transformedDirection.addInPlace(_target);
        auto distanceSquared = Vector3::DistanceSquared(_transformedDirection, panningOriginTarget);
        if (distanceSquared <= (*panningDistanceLimit * *panningDistanceLimit)) {
          _target.copyFrom(_transformedDirection);
        }
      }
      else {
        _target.addInPlace(_transformedDirection);
      }
    }

    inertialPanningX *= panningInertia;
    inertialPanningY *= panningInertia;

    if (std::abs(inertialPanningX) < speed * Math::Epsilon) {
      inertialPanningX = 0.f;
    }
    if (std::abs(inertialPanningY) < speed * Math::Epsilon) {
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

  if (lowerAlphaLimit.has_value() && alpha < lowerAlphaLimit) {
    alpha = *lowerAlphaLimit;
  }
  if (upperAlphaLimit.has_value() && alpha > upperAlphaLimit) {
    alpha = *upperAlphaLimit;
  }

  if (lowerRadiusLimit.has_value() && radius < lowerRadiusLimit) {
    radius               = *lowerRadiusLimit;
    inertialRadiusOffset = 0.f;
  }
  if (upperRadiusLimit.has_value() && radius > upperRadiusLimit) {
    radius               = *upperRadiusLimit;
    inertialRadiusOffset = 0.f;
  }
}

void ArcRotateCamera::rebuildAnglesAndRadius()
{
  _position.subtractToRef(_getTargetPosition(), _computationVector);

  // need to rotate to Y up equivalent if up vector not Axis.Y
  if (_upVector.x != 0.f || _upVector.y != 1.f || _upVector.z != 0.f) {
    Vector3::TransformCoordinatesToRef(_computationVector, _upToYMatrix, _computationVector);
  }

  radius = _computationVector.length();

  if (radius == 0.f) {
    radius = 0.0001f; // Just to avoid division by zero
  }

  // Alpha
  if (_computationVector.x == 0.f && _computationVector.z == 0.f) {
    alpha = Math::PI_2; // avoid division by zero when looking along up axis,
                        // and set to acos(0)
  }
  else {
    alpha = std::acos(
      _computationVector.x
      / std::sqrt(std::pow(_computationVector.x, 2.f) + std::pow(_computationVector.z, 2.f)));
  }

  if (_computationVector.z < 0.f) {
    alpha = Math::PI2 - alpha;
  }

  // Beta
  beta = std::acos(_computationVector.y / radius);

  _checkLimits();
}

void ArcRotateCamera::setPosition(const Vector3& iPosition)
{
  if (_position.equals(iPosition)) {
    return;
  }

  _position.copyFrom(iPosition);

  rebuildAnglesAndRadius();
}

void ArcRotateCamera::setTarget(const std::variant<AbstractMeshPtr, Vector3>& iTarget,
                                bool toBoundingCenter, bool allowSamePosition)
{
  if (std::holds_alternative<AbstractMeshPtr>(iTarget)) {
    const auto& newTarget = std::get<AbstractMeshPtr>(iTarget);
    if (toBoundingCenter) {
      _targetBoundingCenter = newTarget->getBoundingInfo()->boundingBox.centerWorld.clone();
    }
    else {
      _targetBoundingCenter.reset(nullptr);
    }
    newTarget->computeWorldMatrix();
    _targetHost = newTarget;
    _target     = _getTargetPosition();

    onMeshTargetChangedObservable.notifyObservers(_targetHost.get());
  }
  else {
    const auto& newTarget = std::get<Vector3>(iTarget);
    auto currentTarget    = _getTargetPosition();
    if (!allowSamePosition && currentTarget.equals(newTarget)) {
      return;
    }
    _targetHost           = nullptr;
    _target               = newTarget;
    _targetBoundingCenter = nullptr;
    onMeshTargetChangedObservable.notifyObservers(nullptr);
  }

  rebuildAnglesAndRadius();
}

Matrix ArcRotateCamera::_getViewMatrix()
{
  // Compute
  const auto cosa = std::cos(alpha);
  const auto sina = std::sin(alpha);
  const auto cosb = std::cos(beta);
  auto sinb       = std::sin(beta);

  if (sinb == 0.f) {
    sinb = 0.0001f;
  }

  if (radius == 0.f) {
    radius = 0.0001; // Just to avoid division by zero
  }

  auto targetPostion = _getTargetPosition();
  _computationVector.copyFromFloats(radius * cosa * sinb, radius * cosb, radius * sina * sinb);

  // Rotate according to up vector
  if (_upVector.x != 0.f || _upVector.y != 1.f || _upVector.z != 0.f) {
    Vector3::TransformCoordinatesToRef(_computationVector, _YToUpMatrix, _computationVector);
  }

  targetPostion.addToRef(_computationVector, _newPosition);
  if (getScene()->collisionsEnabled && checkCollisions) {
    auto& coordinator = getScene()->collisionCoordinator();
    if (!_collider) {
      _collider = coordinator->createCollider();
    }
    _collider->_radius = *collisionRadius;
    _newPosition.subtractToRef(_position, _collisionVelocity);
    _collisionTriggered = true;
    coordinator->getNewPosition(
      _position, _collisionVelocity, _collider, 3, nullptr,
      [&](size_t collisionId, Vector3& newPosition, const AbstractMeshPtr& collidedMesh) {
        _onCollisionPositionChange(collisionId, newPosition, collidedMesh);
      },
      static_cast<unsigned>(uniqueId));
  }
  else {
    _position.copyFrom(_newPosition);

    auto up = upVector();
    if (allowUpsideDown && sinb < 0.f) {
      up = up.negate();
    }

    _computeViewMatrix(_position, target, up);

    _viewMatrix.addAtIndex(12, targetScreenOffset.x);
    _viewMatrix.addAtIndex(13, targetScreenOffset.y);
  }
  _currentTarget = targetPostion;

  return _viewMatrix;
}

void ArcRotateCamera::_onCollisionPositionChange(size_t /*collisionId*/, Vector3& newPosition,
                                                 const AbstractMeshPtr& collidedMesh)
{
  if (!collidedMesh) {
    _previousPosition.copyFrom(_position);
  }
  else {
    setPosition(newPosition);

    if (onCollide) {
      onCollide(collidedMesh);
    }
  }

  // Recompute because of constraints
  const auto cosa = std::cos(alpha);
  const auto sina = std::sin(alpha);
  const auto cosb = std::cos(beta);
  auto sinb       = std::sin(beta);

  if (sinb == 0.f) {
    sinb = 0.0001f;
  }

  auto targetPostion = _getTargetPosition();
  _computationVector.copyFromFloats(radius * cosa * sinb, radius * cosb, radius * sina * sinb);
  targetPostion.addToRef(_computationVector, _newPosition);
  _position.copyFrom(_newPosition);

  auto up = upVector();
  if (allowUpsideDown && beta < 0.f) {
    up = up.negate();
  }

  _computeViewMatrix(_position, target, up);
  _viewMatrix.addAtIndex(12, targetScreenOffset.x);
  _viewMatrix.addAtIndex(13, targetScreenOffset.y);

  _collisionTriggered = false;
}

void ArcRotateCamera::zoomOn(const std::vector<AbstractMeshPtr>& meshes, bool doNotUpdateMaxZ)
{
  auto _meshes = meshes.empty() ? getScene()->getMeshes() : meshes;

  auto minMaxVector = Mesh::GetMinMax(_meshes);
  auto distance     = Vector3::Distance(minMaxVector.min, minMaxVector.max);

  radius = distance * zoomOnFactor;

  focusOn({minMaxVector.min, minMaxVector.max, distance}, doNotUpdateMaxZ);
}

void ArcRotateCamera::focusOn(const MinMaxDistance& meshesOrMinMaxVectorAndDistance,
                              bool doNotUpdateMaxZ)
{
  _target = Mesh::Center(meshesOrMinMaxVectorAndDistance);

  if (!doNotUpdateMaxZ) {
    maxZ = meshesOrMinMaxVectorAndDistance.distance * 2.f;
  }
}

CameraPtr ArcRotateCamera::createRigCamera(const std::string& iName, int cameraIndex)
{
  float alphaShift = 0.f;

  switch (cameraRigMode) {
    case Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH:
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL:
    case Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER:
    case Camera::RIG_MODE_VR:
      alphaShift = _cameraRigParams.stereoHalfAngle * (cameraIndex == 0 ? 1.f : -1.f);
      break;
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED:
      alphaShift = _cameraRigParams.stereoHalfAngle * (cameraIndex == 0 ? -1.f : 1.f);
      break;
    default:
      break;
  }

  auto rigCam = ArcRotateCamera::New(iName, alpha + alphaShift, beta, radius, _target, getScene());
  rigCam->_cameraRigParams = {};
  rigCam->isRigCamera      = true;
  rigCam->rigParent        = shared_from_base<ArcRotateCamera>();
  rigCam->upVector         = upVector();
  return rigCam;
}

void ArcRotateCamera::_updateRigCameras()
{
  auto camLeft  = std::static_pointer_cast<ArcRotateCamera>(_rigCameras[0]);
  auto camRight = std::static_pointer_cast<ArcRotateCamera>(_rigCameras[1]);

  camLeft->beta = camRight->beta = beta;

  switch (cameraRigMode) {
    case Camera::RIG_MODE_STEREOSCOPIC_ANAGLYPH:
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_PARALLEL:
    case Camera::RIG_MODE_STEREOSCOPIC_OVERUNDER:
    case Camera::RIG_MODE_VR:
      camLeft->alpha  = alpha - _cameraRigParams.stereoHalfAngle;
      camRight->alpha = alpha + _cameraRigParams.stereoHalfAngle;
      break;
    case Camera::RIG_MODE_STEREOSCOPIC_SIDEBYSIDE_CROSSEYED:
      camLeft->alpha  = alpha + _cameraRigParams.stereoHalfAngle;
      camRight->alpha = alpha - _cameraRigParams.stereoHalfAngle;
      break;
    default:
      break;
  }
  TargetCamera::_updateRigCameras();
}

void ArcRotateCamera::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  inputs->clear();
  TargetCamera::dispose(doNotRecurse, disposeMaterialAndTextures);
}

std::string ArcRotateCamera::getClassName() const
{
  return "ArcRotateCamera";
}

json ArcRotateCamera::serialize() const
{
  return json();
}

} // end of namespace BABYLON
