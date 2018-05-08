#include <babylon/animations/runtime_animation.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/animations/ianimation_key.h>

namespace BABYLON {

RuntimeAnimation::RuntimeAnimation(IAnimatable* target, Animation* animation,
                                   Scene* scene, Animatable* host)
    : currentFrame{this, &RuntimeAnimation::get_currentFrame}
    , weight{this, &RuntimeAnimation::get_weight}
    , originalValue{this, &RuntimeAnimation::get_originalValue}
    , currentValue{this, &RuntimeAnimation::get_currentValue}
    , targetPath{this, &RuntimeAnimation::get_targetPath}
    , target{this, &RuntimeAnimation::get_target}
    , _currentFrame{0}
    , _animation{animation}
    , _target{target}
    , _host{host}
    , _stopped{false}
    , _blendingFactor{0.f}
    , _scene{scene}
    , _targetPath{""}
    , _weight{1.f}
    , _ratioOffset{0.f}
    , _previousDelay{millisecond_t{0}}
    , _previousRatio{0.f}
{
  animation->_runtimeAnimations.emplace_back(this);
}

RuntimeAnimation::~RuntimeAnimation()
{
}

int RuntimeAnimation::get_currentFrame() const
{
  return _currentFrame;
}

float RuntimeAnimation::get_weight() const
{
  return _weight;
}

Nullable<AnimationValue>& RuntimeAnimation::get_originalValue()
{
  return _originalValue;
}

Nullable<AnimationValue>& RuntimeAnimation::get_currentValue()
{
  return _currentValue;
}

string_t RuntimeAnimation::get_targetPath() const
{
  return _targetPath;
}

IAnimatable*& RuntimeAnimation::get_target()
{
  return _activeTarget;
}

Animation* RuntimeAnimation::animation()
{
  return _animation;
}

void RuntimeAnimation::reset()
{
  _offsetsCache.clear();
  _highLimitsCache.clear();
  _currentFrame   = 0;
  _blendingFactor = 0;
  _originalValue  = nullptr;
}

bool RuntimeAnimation::isStopped() const
{
  return _stopped;
}

void RuntimeAnimation::dispose()
{
  auto& runtimeAnimations = _animation->runtimeAnimations();
  runtimeAnimations.erase(
    ::std::remove(runtimeAnimations.begin(), runtimeAnimations.end(), this),
    runtimeAnimations.end());
}

AnimationValue
RuntimeAnimation::_interpolate(int /*iCurrentFrame*/, int /*repeatCount*/,
                               unsigned int /*loopMode*/,
                               const AnimationValue& /*offsetValue*/,
                               const AnimationValue& /*highLimitValue*/)
{
  return AnimationValue();
}

void RuntimeAnimation::setValue(const AnimationValue& /*currentValue*/,
                                float weight)
{
  // Set value
  string_t path;
  any destination;

  const auto& targetPropertyPath = _animation->targetPropertyPath;

  if (targetPropertyPath.size() > 1) {
    auto property = _target->getProperty(targetPropertyPath[0]);

    for (size_t index = 1; index < targetPropertyPath.size() - 1; ++index) {
      property = _target->getProperty(property, targetPropertyPath[index]);
    }

    path        = targetPropertyPath.back();
    destination = property;
  }
  else {
    path        = targetPropertyPath[0];
    destination = _target;
  }

  _targetPath   = path;
  _activeTarget = destination;
  _weight       = weight;
}

Nullable<unsigned int> RuntimeAnimation::_getCorrectLoopMode() const
{
  return 0u;
}

void RuntimeAnimation::goToFrame(int frame)
{
  auto& keys = _animation->getKeys();

  int _frame = frame;
  if (_frame < keys[0].frame) {
    _frame = keys[0].frame;
  }
  else if (_frame > keys.back().frame) {
    _frame = keys.back().frame;
  }

  auto currentValue = _interpolate(_frame, 0, _animation->loopMode);

  setValue(currentValue);
}

void RuntimeAnimation::_prepareForSpeedRatioChange(float newSpeedRatio)
{
  auto newRatio = _previousDelay.count()
                  * (_animation->framePerSecond * newSpeedRatio) / 1000.f;
  _ratioOffset = _previousRatio - newRatio;
}

bool RuntimeAnimation::animate(millisecond_t delay, int from, int to, bool loop,
                               float speedRatio, float /*weight*/)
{
  const auto& targetPropertyPath = _animation->targetPropertyPath;
  if (targetPropertyPath.empty()) {
    _stopped = true;
    return false;
  }

  bool returnValue = true;
  auto& keys       = _animation->getKeys();

  // Adding a start key at frame 0 if missing
  if (keys[0].frame != 0) {
    keys.insert(keys.begin(), IAnimationKey(0, keys[0].value));
  }

  // Check limits
  if (from < keys[0].frame || from > keys.back().frame) {
    from = keys[0].frame;
  }
  if (to < keys[0].frame || to > keys.back().frame) {
    to = keys.back().frame;
  }

  // to and from cannot be the same key
  if (from == to) {
    if (from > keys[0].frame) {
      from--;
    }
    else if (to < keys.back().frame) {
      to++;
    }
  }

  // Compute ratio
  auto range = to - from;
  AnimationValue offsetValue;
  // ratio represents the frame delta between from and to
  float ratio = (static_cast<float>(delay.count())
                 * (_animation->framePerSecond * speedRatio) / 1000.f)
                + _ratioOffset;
  AnimationValue highLimitValue(0.f);

  _previousDelay = delay;
  _previousRatio = ratio;

  if (((to > from && ratio > range) || (from > to && ratio < range)) && !loop) {
    // If we are out of range and not looping get back to caller
    returnValue = false;
    // @TODO FIXME
    // highLimitValue = _getKeyValue(keys.back().value);
  }
  else {
    // Get max value if required
    if (_animation->loopMode != Animation::ANIMATIONLOOPMODE_CYCLE()) {
      string_t keyOffset = ::std::to_string(to) + ::std::to_string(from);
      if (!_offsetsCache.count(keyOffset)) {
        AnimationValue fromValue = _interpolate(
          static_cast<int>(from), 0, Animation::ANIMATIONLOOPMODE_CYCLE());
        AnimationValue toValue = _interpolate(
          static_cast<int>(to), 0, Animation::ANIMATIONLOOPMODE_CYCLE());
        switch (_animation->dataType) {
          // Float
          case Animation::ANIMATIONTYPE_FLOAT():
            _offsetsCache[keyOffset] = toValue - fromValue;
            break;
          // Quaternion
          case Animation::ANIMATIONTYPE_QUATERNION():
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Vector3
          case Animation::ANIMATIONTYPE_VECTOR3():
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Vector2
          case Animation::ANIMATIONTYPE_VECTOR2():
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Size
          case Animation::ANIMATIONTYPE_SIZE():
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          // Color3
          case Animation::ANIMATIONTYPE_COLOR3():
            _offsetsCache[keyOffset] = toValue.subtract(fromValue);
            break;
          default:
            break;
        }

        _highLimitsCache[keyOffset] = toValue;
      }

      highLimitValue = _highLimitsCache[keyOffset];
      offsetValue    = _offsetsCache[keyOffset];
    }
  }

  if (offsetValue.dataType == -1) {
    switch (_animation->dataType) {
      // Float
      case Animation::ANIMATIONTYPE_FLOAT():
        offsetValue = AnimationValue(0.f);
        break;
      // Quaternion
      case Animation::ANIMATIONTYPE_QUATERNION():
        offsetValue = AnimationValue(Quaternion(0.f, 0.f, 0.f, 0.f));
        break;
      // Vector3
      case Animation::ANIMATIONTYPE_VECTOR3():
        offsetValue = AnimationValue(Vector3::Zero());
        break;
      // Vector2
      case Animation::ANIMATIONTYPE_VECTOR2():
        offsetValue = AnimationValue(Vector2::Zero());
        break;
      // Size
      case Animation::ANIMATIONTYPE_SIZE():
        offsetValue = Size::Zero();
        break;
      // Color3
      case Animation::ANIMATIONTYPE_COLOR3():
        offsetValue = AnimationValue(Color3::Black());
        break;
      default:
        break;
    }
  }

  // Compute value
  auto repeatCount = static_cast<int>(ratio / range);
  auto _currentFrame
    = returnValue ? from + static_cast<int>(ratio) % range : to;
  auto currentValue
    = _interpolate(_currentFrame, repeatCount, _animation->loopMode,
                   offsetValue, highLimitValue);

  // Set value
  setValue(currentValue);
  // Check events
  auto& events = _animation->getEvents();
  for (unsigned int index = 0; index < events.size(); ++index) {
    // Make sure current frame has passed event frame and that event frame is
    // within the current range
    // Also, handle both forward and reverse animations
    if ((range > 0 && _currentFrame >= events[index].frame
         && events[index].frame >= from)
        || (range < 0 && _currentFrame <= events[index].frame
            && events[index].frame <= from)) {
      AnimationEvent& event = events[index];
      if (!event.isDone) {
        // If event should be done only once, remove it.
        if (event.onlyOnce) {
          events.erase(events.begin() + index);
          --index;
        }
        event.isDone = true;
        event.action();
      } // Don't do anything if the event has already be done.
    }
    else if (events[index].isDone && !events[index].onlyOnce) {
      // reset event, the animation is looping
      events[index].isDone = false;
    }
  }
  if (!returnValue) {
    _stopped = true;
  }

  return returnValue;
}

} // end of namespace BABYLON
