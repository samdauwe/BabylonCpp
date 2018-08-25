#include <babylon/behaviors/cameras/auto_rotation_behavior.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/time.h>
#include <babylon/events/pointer_event_types.h>

namespace BABYLON {

AutoRotationBehavior::AutoRotationBehavior()
    : zoomStopsAnimation{this, &AutoRotationBehavior::get_zoomStopsAnimation,
                         &AutoRotationBehavior::set_zoomStopsAnimation}
    , idleRotationSpeed{this, &AutoRotationBehavior::get_idleRotationSpeed,
                        &AutoRotationBehavior::set_idleRotationSpeed}
    , idleRotationWaitTime{this,
                           &AutoRotationBehavior::get_idleRotationWaitTime,
                           &AutoRotationBehavior::set_idleRotationWaitTime}
    , idleRotationSpinupTime{this,
                             &AutoRotationBehavior::get_idleRotationSpinupTime,
                             &AutoRotationBehavior::set_idleRotationSpinupTime}
    , rotationInProgress{this, &AutoRotationBehavior::get_rotationInProgress}
    , _zoomStopsAnimation{false}
    , _idleRotationSpeed{0.05f}
    , _idleRotationWaitTime{2000}
    , _idleRotationSpinupTime{2000}
    , _onPrePointerObservableObserver{nullptr}
    , _onAfterCheckInputsObserver{nullptr}
    , _attachedCamera{nullptr}
    , _isPointerDown{false}
    , _cameraRotationSpeed{0.f}
    , _lastFrameRadius{0.f}
{
}

AutoRotationBehavior::~AutoRotationBehavior()
{
}

const char* AutoRotationBehavior::name() const
{
  return "AutoRotation";
}

void AutoRotationBehavior::set_zoomStopsAnimation(bool flag)
{
  _zoomStopsAnimation = flag;
}

bool AutoRotationBehavior::get_zoomStopsAnimation() const
{
  return _zoomStopsAnimation;
}

void AutoRotationBehavior::set_idleRotationSpeed(float speed)
{
  _idleRotationSpeed = speed;
}

float AutoRotationBehavior::get_idleRotationSpeed() const
{
  return _idleRotationSpeed;
}

void AutoRotationBehavior::set_idleRotationWaitTime(float time)
{
  _idleRotationWaitTime = time;
}

float AutoRotationBehavior::get_idleRotationWaitTime() const
{
  return _idleRotationWaitTime;
}

void AutoRotationBehavior::set_idleRotationSpinupTime(float time)
{
  _idleRotationSpinupTime = time;
}

float AutoRotationBehavior::get_idleRotationSpinupTime() const
{
  return _idleRotationSpinupTime;
}

bool AutoRotationBehavior::get_rotationInProgress() const
{
  return ::std::abs(_cameraRotationSpeed) > 0.f;
}

void AutoRotationBehavior::init()
{
  // Do nothing
}

void AutoRotationBehavior::attach(const ArcRotateCameraPtr& camera)
{
  _attachedCamera = camera;
  auto scene      = _attachedCamera->getScene();

  _onPrePointerObservableObserver = scene->onPrePointerObservable.add(
    [this](PointerInfoPre* pointerInfoPre, EventState&) {
      if (pointerInfoPre->type == PointerEventTypes::POINTERDOWN) {
        _isPointerDown = true;
        return;
      }

      if (pointerInfoPre->type == PointerEventTypes::POINTERUP) {
        _isPointerDown = false;
      }
    });

  _onAfterCheckInputsObserver
    = camera->onAfterCheckInputsObservable.add([this](Camera*, EventState&) {
        auto now = Time::highresTimepointNow();
        auto dt  = 0.f;
        if (_lastFrameTime.has_value()) {
          dt = Time::fpTimeDiff<float, ::std::milli>(now, *_lastFrameTime);
        }
        _lastFrameTime = now;

        // Stop the animation if there is user interaction and the animation
        // should stop for this interaction
        _applyUserInteraction();

        float timeToRotation
          = Time::fpTimeDiff<float, ::std::milli>(now, _lastInteractionTime)
            - _idleRotationWaitTime;
        float scale = ::std::max(
          ::std::min(timeToRotation / (_idleRotationSpinupTime), 1.f), 0.f);
        _cameraRotationSpeed = _idleRotationSpeed * scale;

        // Step camera rotation by rotation speed
        if (_attachedCamera) {
          _attachedCamera->alpha -= _cameraRotationSpeed * (dt / 1000.f);
        }
      });
}

void AutoRotationBehavior::detach()
{
  if (!_attachedCamera) {
    return;
  }
  auto scene = _attachedCamera->getScene();

  if (_onPrePointerObservableObserver) {
    scene->onPrePointerObservable.remove(_onPrePointerObservableObserver);
  }
  _attachedCamera->onAfterCheckInputsObservable.remove(
    _onAfterCheckInputsObserver);
  _attachedCamera = nullptr;
}

bool AutoRotationBehavior::_userIsZooming() const
{
  if (!_attachedCamera) {
    return false;
  }
  return _attachedCamera->inertialRadiusOffset != 0.f;
}

bool AutoRotationBehavior::_shouldAnimationStopForInteraction()
{
  if (!_attachedCamera) {
    return false;
  }

  auto zoomHasHitLimit = false;
  if (stl_util::almost_equal(_lastFrameRadius, _attachedCamera->radius)
      && !stl_util::almost_equal(_attachedCamera->inertialRadiusOffset, 0.f)) {
    zoomHasHitLimit = true;
  }

  // Update the record of previous radius - works as an approx. indicator of
  // hitting radius limits
  _lastFrameRadius = _attachedCamera->radius;
  return _zoomStopsAnimation ? zoomHasHitLimit : _userIsZooming();
}

void AutoRotationBehavior::_applyUserInteraction()
{
  if (_userIsMoving() && !_shouldAnimationStopForInteraction()) {
    _lastInteractionTime = Time::highresTimepointNow();
  }
}

// Tools
bool AutoRotationBehavior::_userIsMoving()
{
  if (!_attachedCamera) {
    return false;
  }

  return !stl_util::almost_equal(_attachedCamera->inertialAlphaOffset, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialBetaOffset, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialRadiusOffset, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialPanningX, 0.f)
         || !stl_util::almost_equal(_attachedCamera->inertialPanningY, 0.f)
         || _isPointerDown;
}

} // end of namespace BABYLON
