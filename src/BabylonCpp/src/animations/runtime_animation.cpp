#include <babylon/animations/runtime_animation.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/animations/ianimatable.h>

namespace BABYLON {

RuntimeAnimation::RuntimeAnimation(IAnimatable* target, Animation* animation)
    : _animation{animation}
    , _target{target}
    , _stopped{false}
    , _blendingFactor{0}
    , _ratioOffset{0.f}
{
  animation->_runtimeAnimations.emplace_back(this);
}

RuntimeAnimation::~RuntimeAnimation()
{
}

Animation* RuntimeAnimation::animation()
{
  return _animation;
}

void RuntimeAnimation::reset()
{
  _offsetsCache.clear();
  _highLimitsCache.clear();
  currentFrame        = 0;
  _blendingFactor     = 0;
  _originalBlendValue = 0.f;
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

AnimationValue RuntimeAnimation::_getKeyValue(const AnimationValue& value) const
{
  return value;
}

AnimationValue RuntimeAnimation::_interpolate(
  int iCurrentFrame, int repeatCount, unsigned int loopMode,
  const AnimationValue& offsetValue, const AnimationValue& highLimitValue)
{
  if (loopMode == Animation::ANIMATIONLOOPMODE_CONSTANT && repeatCount > 0) {
    return highLimitValue.copy();
  }

  currentFrame      = iCurrentFrame;
  auto _repeatCount = static_cast<float>(repeatCount);

  auto& keys = _animation->getKeys();

  // Try to get a hash to find the right key
  int _keysLength   = static_cast<int>(keys.size());
  int startKeyIndex = ::std::max(
    0, ::std::min(_keysLength - 1,
                  static_cast<int>(
                    ::std::floor(_keysLength * (currentFrame - keys[0].frame)
                                 / (keys.back().frame - keys[0].frame))
                    - 1)));

  if (keys[static_cast<unsigned int>(startKeyIndex)].frame >= currentFrame) {
    while (startKeyIndex - 1 >= 0
           && keys[static_cast<unsigned int>(startKeyIndex)].frame
                >= currentFrame) {
      --startKeyIndex;
    }
  }

  for (size_t key = static_cast<size_t>(startKeyIndex); key < keys.size();
       ++key) {
    const auto& endKey = keys[key + 1];

    if (endKey.frame >= currentFrame) {

      const auto& startKey  = keys[key];
      const auto startValue = _getKeyValue(startKey.value);
      const auto endValue   = _getKeyValue(endKey.value);

      bool useTangent  = startKey.outTangent && endKey.inTangent;
      float frameDelta = static_cast<float>(endKey.frame - startKey.frame);

      // gradient : percent of currentFrame between the frame inf and the frame
      // sup
      float gradient
        = static_cast<float>(currentFrame - startKey.frame) / frameDelta;

      // check for easingFunction and correction of gradient
      auto easingFunction = _animation->getEasingFunction();
      if (easingFunction != nullptr) {
        gradient = easingFunction->ease(gradient);
      }

      auto newVale = keys[key].value.copy();

      switch (_animation->dataType) {
        // Float
        case Animation::ANIMATIONTYPE_FLOAT: {
          const auto floatValue
            = useTangent ?
                _animation->floatInterpolateFunctionWithTangents(
                  startValue.floatData, startKey.outTangent * frameDelta,
                  endValue.floatData, endKey.inTangent * frameDelta, gradient) :
                _animation->floatInterpolateFunction(
                  startValue.floatData, endValue.floatData, gradient);
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.floatData = floatValue;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.floatData
                = offsetValue.floatData * _repeatCount + floatValue;
              return newVale;
            default:
              break;
          }
        } break;
        // Quaternion
        case Animation::ANIMATIONTYPE_QUATERNION: {
          const auto quatValue
            = useTangent ?
                _animation->quaternionInterpolateFunctionWithTangents(
                  startValue.quaternionData,
                  (*startKey.outTangent).quaternionData.scale(frameDelta),
                  endValue.quaternionData,
                  (*endKey.inTangent).quaternionData.scale(frameDelta),
                  gradient) :
                _animation->quaternionInterpolateFunction(
                  startValue.quaternionData, endValue.quaternionData, gradient);
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.quaternionData = quatValue;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.quaternionData
                = quatValue.add(offsetValue.quaternionData.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
        } break;
        // Vector3
        case Animation::ANIMATIONTYPE_VECTOR3: {
          const auto vec3Value
            = useTangent ?
                _animation->vector3InterpolateFunctionWithTangents(
                  startValue.vector3Data,
                  (*startKey.outTangent).vector3Data.scale(frameDelta),
                  endValue.vector3Data,
                  (*endKey.inTangent).vector3Data.scale(frameDelta), gradient) :
                _animation->vector3InterpolateFunction(
                  startValue.vector3Data, endValue.vector3Data, gradient);
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.vector3Data = vec3Value;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.vector3Data
                = vec3Value.add(offsetValue.vector3Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
        } break;
        // Vector2
        case Animation::ANIMATIONTYPE_VECTOR2: {
          const auto vec2Value
            = useTangent ?
                _animation->vector2InterpolateFunctionWithTangents(
                  startValue.vector2Data,
                  (*startKey.outTangent).vector2Data.scale(frameDelta),
                  endValue.vector2Data,
                  (*endKey.inTangent).vector2Data.scale(frameDelta), gradient) :
                _animation->vector2InterpolateFunction(
                  startValue.vector2Data, endValue.vector2Data, gradient);
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.vector2Data = vec2Value;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.vector2Data
                = vec2Value.add(offsetValue.vector2Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
        } break;
        // Size
        case Animation::ANIMATIONTYPE_SIZE:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.sizeData = _animation->sizeInterpolateFunction(
                startValue.sizeData, endValue.sizeData, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.sizeData
                = _animation
                    ->sizeInterpolateFunction(startValue.sizeData,
                                              endValue.sizeData, gradient)
                    .add(offsetValue.sizeData.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Color3
        case Animation::ANIMATIONTYPE_COLOR3:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              newVale.color3Data = _animation->color3InterpolateFunction(
                startValue.color3Data, endValue.color3Data, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.color3Data
                = _animation
                    ->color3InterpolateFunction(startValue.color3Data,
                                                endValue.color3Data, gradient)
                    .add(offsetValue.color3Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Matrix
        case Animation::ANIMATIONTYPE_MATRIX:
          switch (loopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE:
            case Animation::ANIMATIONLOOPMODE_CONSTANT:
              if (Animation::AllowMatricesInterpolation()) {
                newVale.matrixData = _animation->matrixInterpolateFunction(
                  startValue.matrixData, endValue.matrixData, gradient);
                return newVale;
              }
              newVale.matrixData = startValue.matrixData;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE:
              newVale.matrixData = startValue.matrixData;
              return newVale;
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    }
  }
  return _getKeyValue(keys.back().value);
}

void RuntimeAnimation::setValue(const AnimationValue& currentValue,
                                bool /*blend*/)
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

  // Blending
  if (_animation->enableBlending && _blendingFactor <= 1.f) {
  }
  else {
    any newValue = currentValue.getValue();
    _target->setProperty(destination, path, newValue);
  }
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
                               float speedRatio)
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
    keys.insert(keys.begin(), AnimationKey(0, keys[0].value));
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
    returnValue    = false;
    highLimitValue = _getKeyValue(keys.back().value);
  }
  else {
    // Get max value if required
    if (_animation->loopMode != Animation::ANIMATIONLOOPMODE_CYCLE) {
      string_t keyOffset = ::std::to_string(to) + ::std::to_string(from);
      if (!_offsetsCache.count(keyOffset)) {
        AnimationValue fromValue = _interpolate(
          static_cast<int>(from), 0, Animation::ANIMATIONLOOPMODE_CYCLE);
        AnimationValue toValue = _interpolate(
          static_cast<int>(to), 0, Animation::ANIMATIONLOOPMODE_CYCLE);
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
  }

  if (offsetValue.dataType == -1) {
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
    if ((range > 0 && currentFrame >= events[index].frame
         && events[index].frame >= from)
        || (range < 0 && currentFrame <= events[index].frame
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
