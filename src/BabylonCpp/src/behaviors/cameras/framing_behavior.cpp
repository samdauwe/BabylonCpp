#include <babylon/behaviors/cameras/framing_behavior.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/time.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/pointer_event_types.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

ExponentialEase FramingBehavior::_EasingFunction = ExponentialEase();
unsigned int FramingBehavior::EasingMode = EasingFunction::EASINGMODE_EASEINOUT;

FramingBehavior::FramingBehavior()
    : _mode{FramingBehavior::FitFrustumSidesMode}
    , _radiusScale{1.f}
    , _positionScale{0.5f}
    , _defaultElevation{0.3f}
    , _elevationReturnTime{1500}
    , _elevationReturnWaitTime{1000}
    , _zoomStopsAnimation{false}
    , _framingTime{1500}
    , _isPointerDown{false}
    , _betaIsAnimating{false}
    , _betaTransition{nullptr}
    , _radiusTransition{nullptr}
    , _vectorTransition{nullptr}
    , _lastFrameRadius{0.f}
{
}

FramingBehavior::~FramingBehavior()
{
}

const char* FramingBehavior::name() const
{
  return "Framing";
}

void FramingBehavior::setMode(unsigned int mode)
{
  _mode = mode;
}

unsigned int FramingBehavior::mode() const
{
  return _mode;
}

void FramingBehavior::setRadiusScale(float radius)
{
  _radiusScale = radius;
}

float FramingBehavior::radiusScale() const
{
  return _radiusScale;
}

void FramingBehavior::setPositionScale(float scale)
{
  _positionScale = scale;
}

float FramingBehavior::positionScale() const
{
  return _positionScale;
}

void FramingBehavior::setDefaultElevation(float elevation)
{
  _defaultElevation = elevation;
}

float FramingBehavior::defaultElevation() const
{
  return _defaultElevation;
}

void FramingBehavior::setElevationReturnTime(float speed)
{
  _elevationReturnTime = speed;
}

float FramingBehavior::elevationReturnTime() const
{
  return _elevationReturnTime;
}

void FramingBehavior::setElevationReturnWaitTime(float time)
{
  _elevationReturnWaitTime = time;
}

float FramingBehavior::elevationReturnWaitTime() const
{
  return _elevationReturnWaitTime;
}

void FramingBehavior::setZoomStopsAnimation(bool flag)
{
  _zoomStopsAnimation = flag;
}

bool FramingBehavior::zoomStopsAnimation() const
{
  return _zoomStopsAnimation;
}

void FramingBehavior::setFramingTime(float time)
{
  _framingTime = time;
}

float FramingBehavior::framingTime() const
{
  return _framingTime;
}

void FramingBehavior::attach(ArcRotateCamera* camera)
{
  _attachedCamera = camera;
  auto scene      = _attachedCamera->getScene();

  FramingBehavior::_EasingFunction.setEasingMode(FramingBehavior::EasingMode);

  _onPrePointerObservableObserver = scene->onPrePointerObservable.add(
    [this](PointerInfoPre* pointerInfoPre, const EventState&) {
      if (pointerInfoPre->type == PointerEventTypes::POINTERDOWN) {
        _isPointerDown = true;
        return;
      }

      if (pointerInfoPre->type == PointerEventTypes::POINTERUP) {
        _isPointerDown = false;
      }
    });

  _onMeshTargetChangedObserver = camera->onMeshTargetChangedObservable.add(
    [this](AbstractMesh* mesh, const EventState&) {
      if (mesh) {
        zoomOnMesh(mesh);
      }
    });

  _onAfterCheckInputsObserver = camera->onAfterCheckInputsObservable.add(
    [this](Camera*, const EventState&) {
      // Stop the animation if there is user interaction and the animation
      // should stop for this interaction
      _applyUserInteraction();

      // Maintain the camera above the ground. If the user pulls the camera
      // beneath the ground plane, lift it back to the default position after
      // a given timeout
      _maintainCameraAboveGround();
    });
}

void FramingBehavior::detach()
{
  auto scene = _attachedCamera->getScene();

  scene->onPrePointerObservable.remove(_onPrePointerObservableObserver);
  _attachedCamera->onAfterCheckInputsObservable.remove(
    _onAfterCheckInputsObserver);
  _attachedCamera->onMeshTargetChangedObservable.remove(
    _onMeshTargetChangedObserver);

  _attachedCamera = nullptr;
}

void FramingBehavior::zoomOnMesh(AbstractMesh* mesh, bool focusOnOriginXZ)
{
  mesh->computeWorldMatrix(true);

  const auto& boundingBox = mesh->getBoundingInfo()->boundingBox;
  zoomOnBoundingInfo(boundingBox.minimumWorld, boundingBox.maximumWorld,
                     focusOnOriginXZ);
}

void FramingBehavior::zoomOnBoundingInfo(const Vector3& minimumWorld,
                                         const Vector3& maximumWorld,
                                         bool focusOnOriginXZ)
{
  Vector3 zoomTarget;

  // Find target by interpolating from bottom of bounding box in world-space to
  // top via framingPositionY
  auto bottom      = minimumWorld.y;
  auto top         = maximumWorld.y;
  auto zoomTargetY = bottom + (top - bottom) * _positionScale;
  auto radiusWorld = maximumWorld.subtract(minimumWorld).scale(0.5f);

  if (focusOnOriginXZ) {
    zoomTarget = Vector3(0, zoomTargetY, 0);
  }
  else {
    auto centerWorld = minimumWorld.add(radiusWorld);
    zoomTarget       = Vector3(centerWorld.x, zoomTargetY, centerWorld.z);
  }

  if (!_vectorTransition) {
    _vectorTransition
      = Animation::CreateAnimation("target", Animation::ANIMATIONTYPE_VECTOR3,
                                   60, &FramingBehavior::_EasingFunction);
  }

  _betaIsAnimating = true;
  _animatables.emplace_back(Animation::TransitionTo(
    "target", zoomTarget, _attachedCamera, _attachedCamera->getScene(), 60,
    _vectorTransition, _framingTime));

  // sets the radius and lower radius bounds
  // Small delta ensures camera is not always at lower zoom limit.
  auto radius = 0.f;
  if (_mode == FramingBehavior::FitFrustumSidesMode) {
    auto position = _calculateLowerRadiusFromModelBoundingSphere(minimumWorld,
                                                                 maximumWorld);
    _attachedCamera->lowerRadiusLimit
      = radiusWorld.length() + _attachedCamera->minZ;
    radius = position;
  }
  else if (_mode == FramingBehavior::IgnoreBoundsSizeMode) {
    radius = _calculateLowerRadiusFromModelBoundingSphere(minimumWorld,
                                                          maximumWorld);
    if (_attachedCamera->lowerRadiusLimit == 0.f) {
      _attachedCamera->lowerRadiusLimit = _attachedCamera->minZ;
    }
  }

  // transition to new radius
  if (!_radiusTransition) {
    _radiusTransition
      = Animation::CreateAnimation("radius", Animation::ANIMATIONTYPE_FLOAT, 60,
                                   &FramingBehavior::_EasingFunction);
  }

  _animatables.emplace_back(Animation::TransitionTo(
    "radius", radius, _attachedCamera, _attachedCamera->getScene(), 60,
    _radiusTransition, _framingTime));
}

float FramingBehavior::_calculateLowerRadiusFromModelBoundingSphere(
  const Vector3& minimumWorld, const Vector3& maximumWorld)
{
  auto size                    = maximumWorld.subtract(minimumWorld);
  auto boxVectorGlobalDiagonal = size.length();
  auto frustumSlope            = _getFrustumSlope();

  // Formula for setting distance
  // (Good explanation:
  // http://stackoverflow.com/questions/2866350/move-camera-to-fit-3d-scene)
  auto radiusWithoutFraming = boxVectorGlobalDiagonal * 0.5f;

  // Horizon distance
  auto radius = radiusWithoutFraming * _radiusScale;
  auto distanceForHorizontalFrustum
    = radius * ::std::sqrt(1.f + 1.f / (frustumSlope.x * frustumSlope.x));
  auto distanceForVerticalFrustum
    = radius * ::std::sqrt(1.f + 1.f / (frustumSlope.y * frustumSlope.y));
  auto distance
    = ::std::max(distanceForHorizontalFrustum, distanceForVerticalFrustum);
  auto camera = _attachedCamera;

  if (camera->lowerRadiusLimit != 0.f
      && _mode == FramingBehavior::IgnoreBoundsSizeMode) {
    // Don't exceed the requested limit
    distance = distance < camera->lowerRadiusLimit ? camera->lowerRadiusLimit :
                                                     distance;
  }

  // Don't exceed the upper radius limit
  if (camera->upperRadiusLimit != 0.f) {
    distance = distance > camera->upperRadiusLimit ? camera->upperRadiusLimit :
                                                     distance;
  }

  return distance;
}

void FramingBehavior::_maintainCameraAboveGround()
{
  auto now = Time::highresTimepointNow();
  auto timeSinceInteraction
    = Time::fpTimeDiff<float, ::std::milli>(now, _lastInteractionTime);
  auto defaultBeta = Math::PI * 0.5f - _defaultElevation;
  auto limitBeta   = Math::PI * 0.5f;

  // Bring the camera back up if below the ground plane
  if (!_betaIsAnimating && _attachedCamera->beta > limitBeta
      && timeSinceInteraction >= _elevationReturnWaitTime) {
    _betaIsAnimating = true;

    // Transition to new position
    stopAllAnimations();

    if (!_betaTransition) {
      _betaTransition
        = Animation::CreateAnimation("beta", Animation::ANIMATIONTYPE_FLOAT, 60,
                                     &FramingBehavior::_EasingFunction);
    }

    _animatables.emplace_back(Animation::TransitionTo(
      "beta", defaultBeta, _attachedCamera, _attachedCamera->getScene(), 60,
      _betaTransition, _elevationReturnTime, [this]() {
        _clearAnimationLocks();
        stopAllAnimations();
      }));
  }
}

Vector2 FramingBehavior::_getFrustumSlope() const
{
  // Calculate the viewport ratio
  // Aspect Ratio is Height/Width.
  auto camera      = _attachedCamera;
  auto engine      = camera->getScene()->getEngine();
  auto aspectRatio = engine->getAspectRatio(camera);

  // Camera FOV is the vertical field of view (top-bottom) in radians.
  // Slope of the frustum top/bottom planes in view space, relative to the
  // forward vector.
  auto frustumSlopeY = ::std::tan(camera->fov / 2);

  // Slope of the frustum left/right planes in view space, relative to the
  // forward vector. Provides the amount that one side (e.g. left) of the
  // frustum gets wider for every unit along the forward vector.
  auto frustumSlopeX = frustumSlopeY * aspectRatio;

  return Vector2(frustumSlopeX, frustumSlopeY);
}

void FramingBehavior::_clearAnimationLocks()
{
  _betaIsAnimating = false;
}

void FramingBehavior::_applyUserInteraction()
{
  if (isUserIsMoving()) {
    _lastInteractionTime = Time::highresTimepointNow();
    stopAllAnimations();
    _clearAnimationLocks();
  }
}

void FramingBehavior::stopAllAnimations()
{
  _attachedCamera->animations.clear();
  for (auto& animatable : _animatables) {
    animatable->onAnimationEnd = nullptr;
    animatable->stop();
  }
  _animatables.clear();
}

bool FramingBehavior::isUserIsMoving() const
{
  return !stl_util::almost_equal(_attachedCamera->inertialAlphaOffset, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialBetaOffset, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialRadiusOffset, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialPanningX, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialPanningY, 0.f)
         || _isPointerDown;
}

} // end of namespace BABYLON
