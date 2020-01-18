#include <babylon/animations/runtime_animation.h>

#include <cmath>

#include <babylon/animations/_ianimation_state.h>
#include <babylon/animations/animatable.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_properties_override.h>
#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/babylon_stl_util.h>

namespace BABYLON {

RuntimeAnimation::RuntimeAnimation(const IAnimatablePtr& iTarget, const AnimationPtr& animation,
                                   Scene* scene, Animatable* host)
    : currentFrame{this, &RuntimeAnimation::get_currentFrame}
    , weight{this, &RuntimeAnimation::get_weight}
    , currentValue{this, &RuntimeAnimation::get_currentValue}
    , targetPath{this, &RuntimeAnimation::get_targetPath}
    , target{this, &RuntimeAnimation::get_target}
    , _onLoop{nullptr}
    , _currentFrame{0}
    , _originalBlendValue{std::nullopt}
    , _stopped{false}
    , _blendingFactor{0.f}
    , _currentValue{std::nullopt}
    , _currentActiveTarget{nullptr}
    , _directTarget{nullptr}
    , _targetPath{""}
    , _weight{1.f}
    , _ratioOffset{0.f}
    , _previousDelay{millisecond_t{0}}
    , _previousRatio{0.f}
    , _targetIsArray{false}
{
  _animation     = animation;
  _target        = iTarget;
  _scene         = scene;
  _host          = host;
  _activeTargets = {};

  // State
  _animationState.key         = 0;
  _animationState.repeatCount = 0;
  _animationState.loopMode    = _getCorrectLoopMode();

  if (_animation->dataType == static_cast<int>(Animation::ANIMATIONTYPE_MATRIX)) {
    _animationState.workValue = Matrix::Zero();
  }

  // Limits
  _keys     = _animation->getKeys();
  _minFrame = _keys[0].frame;
  _maxFrame = _keys.back().frame;
  _minValue = _keys[0].value;
  _maxValue = _keys.back().value;

  // Add a start key at frame 0 if missing
  if (_minFrame != 0.f) {
    const auto newKey = IAnimationKey{
      0.f,       // frame
      _minValue, // value
    };
    _keys.insert(_keys.begin(), newKey);
  }

  // Check data
  {
    _preparePath(_target);
    _getOriginalValues();
    _targetIsArray = false;
    _directTarget  = _activeTargets[0];
  }

  // Cloning events locally
  const auto& events = animation->getEvents();
  if (!events.empty()) {
    for (const auto& e : events) {
      _events.emplace_back(e);
    }
  }

  _enableBlending = iTarget && iTarget->animationPropertiesOverride() ?
                      iTarget->animationPropertiesOverride()->enableBlending :
                      _animation->enableBlending;
}

RuntimeAnimation::~RuntimeAnimation() = default;

void RuntimeAnimation::addToRuntimeAnimations(const RuntimeAnimationPtr& animation)
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

std::optional<AnimationValue>& RuntimeAnimation::get_currentValue()
{
  return _currentValue;
}

std::string RuntimeAnimation::get_targetPath() const
{
  return _targetPath;
}

IAnimatablePtr& RuntimeAnimation::get_target()
{
  return _currentActiveTarget;
}

void RuntimeAnimation::_preparePath(const IAnimatablePtr& iTarget, unsigned int targetIndex)
{
  const auto& targetPropertyPath = _animation->targetPropertyPath;

  if (targetIndex >= _activeTargets.size()) {
    _activeTargets.resize(targetIndex + 1);
  }

  if (targetPropertyPath.size() > 1) {
    _targetPath                 = targetPropertyPath.back();
    _activeTargets[targetIndex] = iTarget;
  }
  else {
    _targetPath                 = targetPropertyPath[0];
    _activeTargets[targetIndex] = iTarget;
  }
}

AnimationPtr& RuntimeAnimation::animation()
{
  return _animation;
}

void RuntimeAnimation::reset(bool restoreOriginal)
{
  if (restoreOriginal) {
    if (!_originalValue.empty() && _originalValue[0] != std::nullopt) {
      _setValue(_target, _directTarget, *_originalValue[0], -1.f, 0);
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
  stl_util::remove_vector_elements_equal_sharedptr(_animation->runtimeAnimations(), this);
}

void RuntimeAnimation::setValue(const AnimationValue& iCurrentValue, float iWeight)
{
  _setValue(_target, _directTarget, iCurrentValue, iWeight);
}

void RuntimeAnimation::_getOriginalValues(unsigned int targetIndex)
{
  if (targetIndex >= _activeTargets.size()) {
    return;
  }

  if (targetIndex >= _originalValue.size()) {
    _originalValue.resize(targetIndex + 1);
  }

  AnimationValue originalValue;
  auto& iTarget = _activeTargets[targetIndex];

  if (iTarget->getRestPose().has_value() && _targetPath == "_matrix") { // For bones
    originalValue = *iTarget->getRestPose();
  }
  else {
    originalValue = iTarget->getProperty({_targetPath});
  }

  if (originalValue) {
    _originalValue[targetIndex] = originalValue;
  }
  else {
    _originalValue[targetIndex] = originalValue;
  }
}

void RuntimeAnimation::_setValue(const IAnimatablePtr& iTarget, const IAnimatablePtr& destination,
                                 const AnimationValue& iCurrentValue, float iWeight,
                                 unsigned int targetIndex)
{
  // Set value
  _currentActiveTarget = destination;
  _weight              = iWeight;

  if (targetIndex >= _originalValue.size()) {
    _originalValue.resize(targetIndex + 1);
    _originalValue[targetIndex] = std::nullopt;
  }

  // Blending
  auto enableBlending = false;
  if (enableBlending && _blendingFactor <= 1.f) {
    // @TODO: implement
  }
  else {
    _currentValue = iCurrentValue;
  }

  if (!stl_util::almost_equal(iWeight, -1.f)) {
  }
  else {
    if (_currentValue.has_value()) {
      const auto& targetPropertyPath = _animation->targetPropertyPath;
      destination->setProperty(targetPropertyPath, _currentValue.value());
    }
  }

  /* if (iTarget->markAsDirty) */ {
    iTarget->markAsDirty(_animation->targetProperty);
  }
}

std::optional<unsigned int> RuntimeAnimation::_getCorrectLoopMode() const
{
  if (_target && _target->animationPropertiesOverride()) {
    return _target->animationPropertiesOverride()->loopMode;
  }

  return _animation->loopMode;
}

void RuntimeAnimation::goToFrame(float frame)
{
  auto& keys = _animation->getKeys();

  if (frame < keys[0].frame) {
    frame = keys[0].frame;
  }
  else if (frame > keys.back().frame) {
    frame = keys.back().frame;
  }

  // Need to reset animation events
  auto& events = _events;
  if (!events.empty()) {
    for (auto& event : events) {
      if (!event.onlyOnce) {
        // reset events in the future
        event.isDone = event.frame < frame;
      }
    }
  }

  _currentFrame      = frame;
  auto iCurrentValue = _animation->_interpolate(frame, _animationState);

  setValue(iCurrentValue, -1);
}

void RuntimeAnimation::_prepareForSpeedRatioChange(float newSpeedRatio)
{
  auto newRatio = _previousDelay.count() * (_animation->framePerSecond * newSpeedRatio) / 1000.f;
  _ratioOffset  = _previousRatio - newRatio;
}

bool RuntimeAnimation::animate(millisecond_t delay, float from, float to, bool loop,
                               float speedRatio, float iWeight)
{
  auto& animation                = *_animation;
  const auto& targetPropertyPath = animation.targetPropertyPath;
  if (targetPropertyPath.empty()) {
    _stopped = true;
    return false;
  }

  auto returnValue = true;

  // Check limits
  if (from < _minFrame || from > _maxFrame) {
    from = _minFrame;
  }
  if (to < _minFrame || to > _maxFrame) {
    to = _maxFrame;
  }

  // Compute ratio
  const auto range = to - from;
  AnimationValue offsetValue;

  // Compute ratio which represents the frame delta between from and to
  const auto ratio
    = (static_cast<float>(delay.count()) * (animation.framePerSecond * speedRatio) / 1000.f)
      + _ratioOffset;
  AnimationValue highLimitValue(0.f);

  _previousDelay = delay;
  _previousRatio = ratio;

  if (!loop && (to >= from && ratio >= range)) {
    // If we are out of range and not looping get back to caller
    returnValue    = false;
    highLimitValue = animation._getKeyValue(_minValue);
  }
  else if (!loop && (from >= to && ratio <= range)) {
    returnValue    = false;
    highLimitValue = animation._getKeyValue(_minValue);
  }
  else if (_animationState.loopMode != Animation::ANIMATIONLOOPMODE_CYCLE) {
    std::string keyOffset = std::to_string(to) + std::to_string(from);
    if (!_offsetsCache.count(keyOffset)) {
      _animationState.repeatCount = 0;
      _animationState.loopMode    = Animation::ANIMATIONLOOPMODE_CYCLE;
      auto fromValue              = animation._interpolate(from, _animationState);
      auto toValue                = animation._interpolate(to, _animationState);

      _animationState.loopMode = _getCorrectLoopMode();
      switch (_animation->dataType) {
        // Float
        case Animation::ANIMATIONTYPE_FLOAT:
          _offsetsCache[keyOffset] = toValue - fromValue;
          break;
        // Quaternion
        case Animation::ANIMATIONTYPE_QUATERNION:
          _offsetsCache[keyOffset] = toValue.subtract(fromValue);
          break;
        // Vector3
        case Animation::ANIMATIONTYPE_VECTOR3:
          _offsetsCache[keyOffset] = toValue.subtract(fromValue);
          break;
        // Vector2
        case Animation::ANIMATIONTYPE_VECTOR2:
          _offsetsCache[keyOffset] = toValue.subtract(fromValue);
          break;
        // Size
        case Animation::ANIMATIONTYPE_SIZE:
          _offsetsCache[keyOffset] = toValue.subtract(fromValue);
          break;
        // Color3
        case Animation::ANIMATIONTYPE_COLOR3:
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

  auto animationType = offsetValue.animationType();
  if (!animationType.has_value()) {
    switch (_animation->dataType) {
      // Float
      case Animation::ANIMATIONTYPE_FLOAT:
        offsetValue = AnimationValue(0.f);
        break;
      // Quaternion
      case Animation::ANIMATIONTYPE_QUATERNION:
        offsetValue = AnimationValue(Quaternion(0.f, 0.f, 0.f, 0.f));
        break;
      // Vector3
      case Animation::ANIMATIONTYPE_VECTOR3:
        offsetValue = AnimationValue(Vector3::Zero());
        break;
      // Vector2
      case Animation::ANIMATIONTYPE_VECTOR2:
        offsetValue = AnimationValue(Vector2::Zero());
        break;
      // Size
      case Animation::ANIMATIONTYPE_SIZE:
        offsetValue = Size::Zero();
        break;
      // Color3
      case Animation::ANIMATIONTYPE_COLOR3:
        offsetValue = AnimationValue(Color3::Black());
        break;
      default:
        break;
    }
  }

  // Compute value
  auto iCurrentFrame = 0.f;

  // Need to normalize?
  if (_host && _host->syncRoot()) {
    auto syncRoot = _host->syncRoot();
    auto hostNormalizedFrame
      = (syncRoot->masterFrame - syncRoot->fromFrame) / (syncRoot->toFrame - syncRoot->fromFrame);
    iCurrentFrame = from + (to - from) * hostNormalizedFrame;
  }
  else {
    iCurrentFrame = (returnValue && range != 0.f) ? from + std::fmod(ratio, range) : to;
  }

  // Reset events if looping
  auto& events = _events;
  if ((range > 0.f && currentFrame > iCurrentFrame)
      || (range < 0.f && currentFrame < iCurrentFrame)) {
    _onLoop();

    // Need to reset animation events
    if (!events.empty()) {
      for (auto& event : events) {
        if (!event.onlyOnce) {
          // reset event, the animation is looping
          event.isDone = false;
        }
      }
    }
  }
  _currentFrame                  = iCurrentFrame;
  _animationState.repeatCount    = range == 0.f ? 0 : static_cast<int>(ratio / range) >> 0;
  _animationState.highLimitValue = highLimitValue;
  _animationState.offsetValue    = offsetValue;

  auto iCurrentValue = animation._interpolate(iCurrentFrame, _animationState);

  // Set value
  setValue(iCurrentValue, iWeight);

  // Check events
  if (!events.empty()) {
    for (unsigned int index = 0; index < events.size(); ++index) {
      // Make sure current frame has passed event frame and that event frame is
      // within the current range
      // Also, handle both forward and reverse animations
      if ((range > 0.f && iCurrentFrame >= events[index].frame && events[index].frame >= from)
          || (range < 0.f && iCurrentFrame <= events[index].frame && events[index].frame <= from)) {
        auto& event = events[index];
        if (!event.isDone) {
          // If event should be done only once, remove it.
          if (event.onlyOnce) {
            events.erase(events.begin() + index);
            --index;
          }
          event.isDone = true;
          event.action(iCurrentFrame);
        } // Don't do anything if the event has already be done.
      }
    }
  }

  if (!returnValue) {
    _stopped = true;
  }

  return returnValue;
}

} // end of namespace BABYLON
