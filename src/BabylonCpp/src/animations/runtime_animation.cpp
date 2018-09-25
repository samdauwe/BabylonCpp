#include <babylon/animations/runtime_animation.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/babylon_stl_util.h>

namespace BABYLON {

RuntimeAnimation::RuntimeAnimation(const IAnimatablePtr& target,
                                   const AnimationPtr& animation, Scene* scene,
                                   Animatable* host)
    : currentFrame{this, &RuntimeAnimation::get_currentFrame}
    , weight{this, &RuntimeAnimation::get_weight}
    , currentValue{this, &RuntimeAnimation::get_currentValue}
    , _workValue{nullopt_t}
    , targetPath{this, &RuntimeAnimation::get_targetPath}
    , target{this, &RuntimeAnimation::get_target}
    , _currentFrame{0}
    , _animation{animation}
    , _target{target}
    , _host{host}
    , _stopped{false}
    , _blendingFactor{0.f}
    , _scene{scene}
    , _currentValue{nullopt_t}
    , _targetPath{""}
    , _weight{1.f}
    , _ratioOffset{0.f}
    , _previousDelay{millisecond_t{0}}
    , _previousRatio{0.f}
{
  // Cloning events locally
  const auto& events = animation->getEvents();
  if (!events.empty()) {
    for (const auto& e : events) {
      _events.emplace_back(e);
    }
  }
}

RuntimeAnimation::~RuntimeAnimation()
{
}

void RuntimeAnimation::addToRuntimeAnimations(
  const RuntimeAnimationPtr& animation)
{
  _animation->_runtimeAnimations.emplace_back(animation);
}

float RuntimeAnimation::get_currentFrame() const
{
  return _currentFrame;
}

float RuntimeAnimation::get_weight() const
{
  return _weight;
}

nullable_t<AnimationValue>& RuntimeAnimation::get_currentValue()
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

AnimationPtr& RuntimeAnimation::animation()
{
  return _animation;
}

void RuntimeAnimation::reset(bool restoreOriginal)
{
  if (restoreOriginal) {
    if (!_originalValue.empty() && _originalValue[0] != nullopt_t) {
      // _setValue(_target, _originalValue[0], -1);
    }
  }

  _offsetsCache.clear();
  _highLimitsCache.clear();
  _currentFrame   = 0;
  _blendingFactor = 0;
  _originalValue.clear();

  // Events
  for (auto& event : _events) {
    event.isDone = false;
  }
}

bool RuntimeAnimation::isStopped() const
{
  return _stopped;
}

void RuntimeAnimation::dispose()
{
  auto& runtimeAnimations = _animation->runtimeAnimations();
  runtimeAnimations.erase(
    std::remove_if(runtimeAnimations.begin(), runtimeAnimations.end(),
                     [this](const RuntimeAnimationPtr& runtimeAnimation) {
                       return runtimeAnimation.get() == this;
                     }),
    runtimeAnimations.end());
}

AnimationValue RuntimeAnimation::_interpolate(
  float iCurrentFrame, int repeatCount, unsigned int loopMode,
  const AnimationValue& offsetValue, const AnimationValue& highLimitValue)
{
  _currentFrame = iCurrentFrame;

  if (_animation->dataType == Animation::ANIMATIONTYPE_MATRIX()
      && !_workValue) {
    _workValue = Matrix::Zero();
  }

  return _animation->_interpolate(currentFrame, repeatCount, _workValue,
                                  loopMode, offsetValue, highLimitValue);
}

void RuntimeAnimation::setValue(const AnimationValue& currentValue,
                                float weight)
{
  _setValue(_target, currentValue, weight);
}

void RuntimeAnimation::_setValue(const IAnimatablePtr& target,
                                 const AnimationValue& currentValue,
                                 float weight, unsigned int targetIndex)
{
  // Set value
  string_t path   = "";
  any destination = nullptr;

  const auto& targetPropertyPath = _animation->targetPropertyPath;

  if (targetPropertyPath.size() > 1) {
    auto property = target->getProperty(targetPropertyPath[0]);

    for (size_t index = 1; index < targetPropertyPath.size() - 1; ++index) {
      property = target->getProperty(property, targetPropertyPath[index]);
    }

    path        = targetPropertyPath.back();
    destination = property;
  }
  else {
    path        = targetPropertyPath[0];
    destination = target;
  }

  _targetPath   = path;
  _activeTarget = destination;
  _weight       = weight;

  if (targetIndex >= _originalValue.size()) {
    _originalValue.resize(targetIndex + 1);
    _originalValue[targetIndex] = nullopt_t;
  }

  // Blending
  auto enableBlending = false;
  if (enableBlending && _blendingFactor <= 1.f) {
  }
  else {
    _currentValue = currentValue;
  }

  if (!stl_util::almost_equal(weight, -1.f)) {
  }
  else {
    any newValue = (*_currentValue).getValue();
    target->setProperty(destination, path, newValue);
  }
}

nullable_t<unsigned int> RuntimeAnimation::_getCorrectLoopMode() const
{
  return 0u;
}

void RuntimeAnimation::goToFrame(int frame)
{
  auto& keys = _animation->getKeys();

  float _frame = frame;
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

bool RuntimeAnimation::animate(millisecond_t delay, float from, float to,
                               bool loop, float speedRatio, float iWeight)
{
  const auto& targetPropertyPath = _animation->targetPropertyPath;
  if (targetPropertyPath.empty()) {
    _stopped = true;
    return false;
  }

  auto returnValue = true;

  auto& keys = _animation->getKeys();

  // Adding a start key at frame 0 if missing
  if (keys[0].frame > 0.f) {
    keys.insert(keys.begin(), IAnimationKey(0, keys[0].value));
  }
  // Adding a duplicate key when there is only one key at frame zero
  else if (keys.size() == 1) {
    keys.emplace_back(IAnimationKey(0.001f, keys[0].value));
  }

  // Check limits
  if (from < keys[0].frame || from > keys.back().frame) {
    from = keys[0].frame;
  }
  if (to < keys[0].frame || to > keys.back().frame) {
    to = keys.back().frame;
  }

  // to and from cannot be the same key
  if (stl_util::almost_equal(from, to)) {
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

  if (((to > from && ratio >= range) || (from > to && ratio <= range))
      && !loop) {
    // If we are out of range and not looping get back to caller
    returnValue = false;
    // @TODO FIXME
    // highLimitValue = _getKeyValue(keys.back().value);
  }
  else {
    // Get max value if required
    if (_animation->loopMode != Animation::ANIMATIONLOOPMODE_CYCLE()) {
      string_t keyOffset = std::to_string(to) + std::to_string(from);
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
  auto repeatCount   = static_cast<int>(ratio / range);
  auto _currentFrame = returnValue ? from + std::fmod(ratio, range) : to;

  // Need to normalize?
  if (_host && _host->syncRoot) {
    auto syncRoot            = _host->syncRoot();
    auto hostNormalizedFrame = (syncRoot->masterFrame - syncRoot->fromFrame)
                               / (syncRoot->toFrame - syncRoot->fromFrame);
    _currentFrame = from + (to - from) * hostNormalizedFrame;
  }

  // Reset events if looping
  auto& events = _events;
  if ((range > 0 && currentFrame > currentFrame)
      || (range < 0 && currentFrame < currentFrame)) {
    // Need to reset animation events
    for (auto& event : events) {
      if (!event.onlyOnce) {
        // reset event, the animation is looping
        event.isDone = false;
      }
    }
  }

  auto currentValue
    = _interpolate(_currentFrame, repeatCount, *_getCorrectLoopMode(),
                   offsetValue, highLimitValue);

  // Set value
  setValue(currentValue, iWeight);

  // Check events
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
        event.action(currentFrame);
      } // Don't do anything if the event has already be done.
    }
  }
  if (!returnValue) {
    _stopped = true;
  }

  return returnValue;
}

} // end of namespace BABYLON
