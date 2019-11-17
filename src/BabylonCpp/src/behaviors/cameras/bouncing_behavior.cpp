#include <babylon/behaviors/cameras/bouncing_behavior.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

BackEasePtr BouncingBehavior::_EasingFunction = BackEase::New(0.3f);
unsigned int BouncingBehavior::EasingMode = EasingFunction::EASINGMODE_EASEOUT;

BouncingBehavior::BouncingBehavior()
    : transitionDuration{450.f}
    , lowerRadiusTransitionRange{2.f}
    , upperRadiusTransitionRange{-2.f}
    , autoTransitionRange{this, &BouncingBehavior::get_autoTransitionRange,
                          &BouncingBehavior::set_autoTransitionRange}
    , _autoTransitionRange{false}
    , _attachedCamera{nullptr}
    , _onAfterCheckInputsObserver{nullptr}
    , _onMeshTargetChangedObserver{nullptr}
    , _radiusIsAnimating{false}
    , _radiusBounceTransition{nullptr}
{
}

BouncingBehavior::~BouncingBehavior() = default;

const char* BouncingBehavior::name() const
{
  return "Bouncing";
}

bool BouncingBehavior::get_autoTransitionRange() const
{
  return _autoTransitionRange;
}

void BouncingBehavior::set_autoTransitionRange(bool value)
{
  if (_autoTransitionRange == value) {
    return;
  }

  _autoTransitionRange = value;

  auto camera = _attachedCamera;
  if (!camera) {
    return;
  }

  if (value) {
    _onMeshTargetChangedObserver = camera->onMeshTargetChangedObservable.add(
      [this](AbstractMesh* mesh, EventState&) {
        if (!mesh) {
          return;
        }

        mesh->computeWorldMatrix(true);
        auto diagonal = mesh->getBoundingInfo()->diagonalLength();

        lowerRadiusTransitionRange = diagonal * 0.05f;
        upperRadiusTransitionRange = diagonal * 0.05f;
      });
  }
  else if (_onMeshTargetChangedObserver) {
    camera->onMeshTargetChangedObservable.remove(_onMeshTargetChangedObserver);
  }
}

void BouncingBehavior::init()
{
  // Do nothing
}

void BouncingBehavior::attach(
  const ArcRotateCameraPtr& camera,
  const std::function<bool(const AbstractMeshPtr& m)>& /*predicate*/)
{
  _attachedCamera = camera;
  _onAfterCheckInputsObserver
    = camera->onAfterCheckInputsObservable.add([this](Camera*, EventState&) {
        if (!_attachedCamera) {
          return;
        }

        // Add the bounce animation to the lower radius limit
        if (_isRadiusAtLimit(*_attachedCamera->lowerRadiusLimit)) {
          _applyBoundRadiusAnimation(lowerRadiusTransitionRange);
        }

        // Add the bounce animation to the upper radius limit
        if (_isRadiusAtLimit(*_attachedCamera->upperRadiusLimit)) {
          _applyBoundRadiusAnimation(upperRadiusTransitionRange);
        }
      });
}

void BouncingBehavior::detach()
{
  if (!_attachedCamera) {
    return;
  }
  if (_onAfterCheckInputsObserver) {
    _attachedCamera->onAfterCheckInputsObservable.remove(
      _onAfterCheckInputsObserver);
  }
  if (_onMeshTargetChangedObserver) {
    _attachedCamera->onMeshTargetChangedObservable.remove(
      _onMeshTargetChangedObserver);
  }
  _attachedCamera = nullptr;
}

bool BouncingBehavior::_isRadiusAtLimit(float radiusLimit) const
{
  if (!_attachedCamera) {
    return false;
  }

  return stl_util::almost_equal(_attachedCamera->radius, radiusLimit)

         && !_radiusIsAnimating;
}

void BouncingBehavior::_applyBoundRadiusAnimation(float radiusDelta)
{
  if (!_attachedCamera) {
    return;
  }

  if (!_radiusBounceTransition) {
    BouncingBehavior::_EasingFunction->setEasingMode(
      BouncingBehavior::EasingMode);
    _radiusBounceTransition
      = Animation::CreateAnimation("radius", Animation::ANIMATIONTYPE_FLOAT(),
                                   60, BouncingBehavior::_EasingFunction);
  }
  // Prevent zoom until bounce has completed
  _cachedWheelPrecision           = _attachedCamera->wheelPrecision;
  _attachedCamera->wheelPrecision = std::numeric_limits<float>::infinity();
  _attachedCamera->inertialRadiusOffset = 0.f;

  // Animate to the radius limit
  stopAllAnimations();
  _radiusIsAnimating = true;
  auto animatable    = Animation::TransitionTo(
    "radius", _attachedCamera->radius + radiusDelta, _attachedCamera.get(),
    _attachedCamera->getScene(), 60, _radiusBounceTransition,
    transitionDuration, [this]() { _clearAnimationLocks(); });

  if (animatable) {
    _animatables.emplace_back(animatable);
  }
}

void BouncingBehavior::_clearAnimationLocks()
{
  _radiusIsAnimating = false;

  if (_attachedCamera) {
    _attachedCamera->wheelPrecision = _cachedWheelPrecision;
  }
}

void BouncingBehavior::stopAllAnimations()
{
  if (_attachedCamera) {
    _attachedCamera->animations.clear();
  }

  for (auto& animatable : _animatables) {
    animatable->onAnimationEnd = nullptr;
    animatable->stop();
  }
  _animatables.clear();
}

} // end of namespace BABYLON
