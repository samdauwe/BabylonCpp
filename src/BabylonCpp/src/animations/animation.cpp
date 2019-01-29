#include <babylon/animations/animation.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/animations/runtime_animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/json_util.h>
#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/scalar.h>
#include <babylon/math/size.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool Animation::_AllowMatricesInterpolation = false;

bool Animation::_AllowMatrixDecomposeForInterpolation = true;

bool Animation::AllowMatricesInterpolation()
{
  return _AllowMatricesInterpolation;
}

bool Animation::AllowMatrixDecomposeForInterpolation()
{
  return _AllowMatrixDecomposeForInterpolation;
}

AnimationPtr Animation::_PrepareAnimation(
  const std::string& name, const std::string& targetProperty,
  size_t framePerSecond, float totalFrame, const AnimationValue& from,
  const AnimationValue& to, unsigned int loopMode,
  const IEasingFunctionPtr& easingFunction)
{
  auto dataType = from.dataType;

  if (dataType == -1) {
    return nullptr;
  }

  auto animation
    = Animation::New(name, targetProperty, framePerSecond, dataType, loopMode);

  animation->setKeys({
    IAnimationKey(0.f, from),
    IAnimationKey(static_cast<float>(totalFrame), to),
  });

  if (easingFunction != nullptr) {
    animation->setEasingFunction(easingFunction);
  }

  return animation;
}

AnimationPtr
Animation::CreateAnimation(const std::string& property, int animationType,
                           std::size_t framePerSecond,
                           const IEasingFunctionPtr& easingFunction)
{
  auto animation
    = Animation::New(property + "Animation", property, framePerSecond,
                     animationType, Animation::ANIMATIONLOOPMODE_CONSTANT());

  animation->setEasingFunction(easingFunction);

  return animation;
}

AnimatablePtr Animation::CreateAndStartAnimation(
  const std::string& name, const NodePtr& node,
  const std::string& targetProperty, size_t framePerSecond, float totalFrame,
  const AnimationValue& from, const AnimationValue& to, unsigned int loopMode,
  const IEasingFunctionPtr& easingFunction,
  const std::function<void()>& onAnimationEnd)
{

  auto animation = Animation::_PrepareAnimation(
    name, targetProperty, framePerSecond, totalFrame, from, to, loopMode,
    easingFunction);

  if (!animation) {
    return nullptr;
  }

  return node->getScene()->beginDirectAnimation(
    node, {animation}, 0.f, static_cast<float>(totalFrame),
    (animation->loopMode == 1), 1.f, onAnimationEnd);
}

std::vector<AnimatablePtr> Animation::CreateAndStartHierarchyAnimation(
  const std::string& name, const NodePtr& node, bool directDescendantsOnly,
  const std::string& targetProperty, size_t framePerSecond, float totalFrame,
  const AnimationValue& from, const AnimationValue& to, unsigned int loopMode,
  const IEasingFunctionPtr& easingFunction,
  const std::function<void()>& onAnimationEnd)
{
  auto animation = Animation::_PrepareAnimation(
    name, targetProperty, framePerSecond, totalFrame, from, to, loopMode,
    easingFunction);

  if (!animation) {
    return {};
  }

  auto scene = node->getScene();
  return scene->beginDirectHierarchyAnimation(
    node, directDescendantsOnly, {animation}, 0.f, totalFrame,
    (animation->loopMode == 1), 1.f, onAnimationEnd);
}

AnimatablePtr Animation::CreateMergeAndStartAnimation(
  const std::string& name, const NodePtr& node,
  const std::string& targetProperty, size_t framePerSecond, float totalFrame,
  const AnimationValue& from, const AnimationValue& to, unsigned int loopMode,
  const IEasingFunctionPtr& easingFunction,
  const std::function<void()>& onAnimationEnd)
{
  auto animation = Animation::_PrepareAnimation(
    name, targetProperty, framePerSecond, totalFrame, from, to, loopMode,
    easingFunction);

  if (!animation) {
    return nullptr;
  }

  node->animations.emplace_back(animation);

  return node->getScene()->beginAnimation(
    node, 0.f, totalFrame, (animation->loopMode == 1), 1.f, onAnimationEnd);
}

Animatable* Animation::TransitionTo(
  const std::string& /*property*/, const AnimationValue& /*targetValue*/,
  const AnimationValue& /*host*/, Scene* /*scene*/, float /*frameRate*/,
  const AnimationPtr& /*transition*/, float /*duration*/,
  const std::function<void()>& /*onAnimationEnd*/)
{
  return nullptr;
}

std::vector<RuntimeAnimationPtr>& Animation::runtimeAnimations()
{
  return _runtimeAnimations;
}

bool Animation::get_hasRunningRuntimeAnimations() const
{
  for (auto& runtimeAnimation : _runtimeAnimations) {
    if (!runtimeAnimation->isStopped()) {
      return true;
    }
  }

  return false;
}

Animation::Animation(const std::string& iName,
                     const std::string& iTargetProperty, size_t iFramePerSecond,
                     int iDataType, unsigned int iLoopMode)

    : name{iName}
    , targetProperty{iTargetProperty}
    , targetPropertyPath{String::split(targetProperty, '.')}
    , framePerSecond{iFramePerSecond}
    , dataType{iDataType}
    , loopMode{iLoopMode}
    , blendingSpeed{0.01f}
    , enableBlending{false}
    , hasRunningRuntimeAnimations{this,
                                  &Animation::get_hasRunningRuntimeAnimations}
    , _easingFunction{nullptr}
{
}

Animation::~Animation()
{
}

std::string Animation::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", property: " << targetProperty;
  if (dataType >= 0
      && static_cast<unsigned int>(dataType) <= ANIMATIONTYPE_BOOL()) {
    size_t _dataType = static_cast<size_t>(dataType);
    oss << ", datatype: "
        << std::vector<std::string>{"Float",  "Vector3", "Quaternion",
                                    "Matrix", "Color3",  "Vector2",
                                    "Size",   "Boolean"}[_dataType];
  }
  oss << ", nKeys: "
      << (!_keys.empty() ? std::to_string(_keys.size()) : "none");
  oss << ", nRanges: "
      << (!_ranges.empty() ? std::to_string(_ranges.size()) : "none");
  if (fullDetails) {
    oss << ", Ranges: {";
    bool first = true;
    for (auto& range : _ranges) {
      if (first) {
        oss << ", ";
        first = false;
      }
      oss << range.first;
    }
    oss << "}";
  }
  return oss.str();
}

void Animation::addEvent(const AnimationEvent& event)
{
  _events.emplace_back(event);
}

void Animation::removeEvents(int frame)
{
  _events.erase(std::remove_if(_events.begin(), _events.end(),
                               [frame](const AnimationEvent& event) {
                                 return event.frame == frame;
                               }),
                _events.end());
}

std::vector<AnimationEvent>& Animation::getEvents()
{
  return _events;
}

void Animation::createRange(const std::string& _name, float from, float to)
{
  // check name not already in use; could happen for bones after serialized
  if (!stl_util::contains(_ranges, _name)) {
    _ranges[name] = AnimationRange(_name, from, to);
  }
}

void Animation::deleteRange(const std::string& iName, bool deleteFrames)
{
  if (stl_util::contains(_ranges, iName)) {
    if (deleteFrames) {
      const auto& from = _ranges[iName].from;
      const auto& to   = _ranges[iName].to;

      _keys.erase(std::remove_if(_keys.begin(), _keys.end(),
                                 [from, to](const IAnimationKey& key) {
                                   return key.frame >= from && key.frame <= to;
                                 }),
                  _keys.end());
    }
    _ranges.erase(iName);
  }
}

AnimationRange& Animation::getRange(const std::string& iName)
{
  return _ranges[iName];
}

std::vector<IAnimationKey>& Animation::getKeys()
{
  return _keys;
}

float Animation::getHighestFrame() const
{
  float ret = 0;
  for (auto& key : _keys) {
    if (ret < key.frame) {
      ret = key.frame;
    }
  }
  return ret;
}

IEasingFunctionPtr& Animation::getEasingFunction()
{
  return _easingFunction;
}

void Animation::setEasingFunction(const IEasingFunctionPtr& easingFunction)
{
  _easingFunction = easingFunction;
}

float Animation::floatInterpolateFunction(float startValue, float endValue,
                                          float gradient) const
{
  return Scalar::Lerp(startValue, endValue, gradient);
}

float Animation::floatInterpolateFunctionWithTangents(float startValue,
                                                      float outTangent,
                                                      float endValue,
                                                      float inTangent,
                                                      float gradient) const
{
  return Scalar::Hermite(startValue, outTangent, endValue, inTangent, gradient);
}

Quaternion
Animation::quaternionInterpolateFunction(const Quaternion& startValue,
                                         const Quaternion& endValue,
                                         float gradient) const
{
  return Quaternion::Slerp(startValue, endValue, gradient);
}

Quaternion Animation::quaternionInterpolateFunctionWithTangents(
  const Quaternion& startValue, const Quaternion& outTangent,
  const Quaternion& endValue, const Quaternion& inTangent, float gradient) const
{
  return Quaternion::Hermite(startValue, outTangent, endValue, inTangent,
                             gradient)
    .normalize();
}

Vector3 Animation::vector3InterpolateFunction(const Vector3& startValue,
                                              const Vector3& endValue,
                                              float gradient) const
{
  return Vector3::Lerp(startValue, endValue, gradient);
}

Vector3 Animation::vector3InterpolateFunctionWithTangents(
  const Vector3& startValue, const Vector3& outTangent, const Vector3& endValue,
  const Vector3& inTangent, float gradient) const
{
  return Vector3::Hermite(startValue, outTangent, endValue, inTangent,
                          gradient);
}

Vector2 Animation::vector2InterpolateFunction(const Vector2& startValue,
                                              const Vector2& endValue,
                                              float gradient) const
{
  return Vector2::Lerp(startValue, endValue, gradient);
}

Vector2 Animation::vector2InterpolateFunctionWithTangents(
  const Vector2& startValue, const Vector2& outTangent, const Vector2& endValue,
  const Vector2& inTangent, float gradient) const
{
  return Vector2::Hermite(startValue, outTangent, endValue, inTangent,
                          gradient);
}

Size Animation::sizeInterpolateFunction(const Size& startValue,
                                        const Size& endValue,
                                        float gradient) const
{
  return Size::Lerp(startValue, endValue, gradient);
}

Color3 Animation::color3InterpolateFunction(const Color3& startValue,
                                            const Color3& endValue,
                                            float gradient) const
{
  return Color3::Lerp(startValue, endValue, gradient);
}

AnimationValue Animation::_getKeyValue(const AnimationValue& value) const
{
  return value;
}

AnimationValue Animation::_interpolate(float currentFrame, int repeatCount,
                                       std::optional<AnimationValue>& workValue,
                                       unsigned int iLoopMode,
                                       const AnimationValue& offsetValue,
                                       const AnimationValue& highLimitValue)
{
  if (loopMode == Animation::ANIMATIONLOOPMODE_CONSTANT() && repeatCount > 0) {
    return highLimitValue.copy();
  }

  auto _repeatCount = static_cast<float>(repeatCount);

  auto& keys = getKeys();

  // Try to get a hash to find the right key
  int _keysLength   = static_cast<int>(keys.size());
  int startKeyIndex = std::max(
    0, std::min(_keysLength - 1,
                static_cast<int>(
                  std::floor(_keysLength * (currentFrame - keys[0].frame)
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

      const auto& startKey = keys[key];
      auto startValue      = _getKeyValue(startKey.value);
      if ((*startKey.interpolation).dataType
          == static_cast<int>(AnimationKeyInterpolation::STEP)) {
        return startValue;
      }
      auto endValue = _getKeyValue(endKey.value);

      bool useTangent  = startKey.outTangent && endKey.inTangent;
      float frameDelta = static_cast<float>(endKey.frame - startKey.frame);

      // gradient : percent of currentFrame between the frame inf and the frame
      // sup
      float gradient
        = static_cast<float>(currentFrame - startKey.frame) / frameDelta;

      // check for easingFunction and correction of gradient
      auto easingFunction = getEasingFunction();
      if (easingFunction != nullptr) {
        gradient = easingFunction->ease(gradient);
      }

      auto newVale = keys[key].value.copy();

      switch (dataType) {
        // Float
        case Animation::ANIMATIONTYPE_FLOAT(): {
          const auto floatValue
            = useTangent ?
                floatInterpolateFunctionWithTangents(
                  startValue.floatData, *startKey.outTangent * frameDelta,
                  endValue.floatData, *endKey.inTangent * frameDelta,
                  gradient) :
                floatInterpolateFunction(startValue.floatData,
                                         endValue.floatData, gradient);
          switch (iLoopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE():
            case Animation::ANIMATIONLOOPMODE_CONSTANT():
              newVale.floatData = floatValue;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE():
              newVale.floatData
                = offsetValue.floatData * _repeatCount + floatValue;
              return newVale;
            default:
              break;
          }
        } break;
        // Quaternion
        case Animation::ANIMATIONTYPE_QUATERNION(): {
          const auto quatValue
            = useTangent ?
                quaternionInterpolateFunctionWithTangents(
                  startValue.quaternionData,
                  (*startKey.outTangent).quaternionData.scale(frameDelta),
                  endValue.quaternionData,
                  (*endKey.inTangent).quaternionData.scale(frameDelta),
                  gradient) :
                quaternionInterpolateFunction(
                  startValue.quaternionData, endValue.quaternionData, gradient);
          switch (iLoopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE():
            case Animation::ANIMATIONLOOPMODE_CONSTANT():
              newVale.quaternionData = quatValue;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE():
              newVale.quaternionData
                = quatValue.add(offsetValue.quaternionData.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
        } break;
        // Vector3
        case Animation::ANIMATIONTYPE_VECTOR3(): {
          const auto vec3Value
            = useTangent ?
                vector3InterpolateFunctionWithTangents(
                  startValue.vector3Data,
                  (*startKey.outTangent).vector3Data.scale(frameDelta),
                  endValue.vector3Data,
                  (*endKey.inTangent).vector3Data.scale(frameDelta), gradient) :
                vector3InterpolateFunction(startValue.vector3Data,
                                           endValue.vector3Data, gradient);
          switch (iLoopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE():
            case Animation::ANIMATIONLOOPMODE_CONSTANT():
              newVale.vector3Data = vec3Value;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE():
              newVale.vector3Data
                = vec3Value.add(offsetValue.vector3Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
        } break;
        // Vector2
        case Animation::ANIMATIONTYPE_VECTOR2(): {
          const auto vec2Value
            = useTangent ?
                vector2InterpolateFunctionWithTangents(
                  startValue.vector2Data,
                  (*startKey.outTangent).vector2Data.scale(frameDelta),
                  endValue.vector2Data,
                  (*endKey.inTangent).vector2Data.scale(frameDelta), gradient) :
                vector2InterpolateFunction(startValue.vector2Data,
                                           endValue.vector2Data, gradient);
          switch (iLoopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE():
            case Animation::ANIMATIONLOOPMODE_CONSTANT():
              newVale.vector2Data = vec2Value;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE():
              newVale.vector2Data
                = vec2Value.add(offsetValue.vector2Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
        } break;
        // Size
        case Animation::ANIMATIONTYPE_SIZE():
          switch (iLoopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE():
            case Animation::ANIMATIONLOOPMODE_CONSTANT():
              newVale.sizeData = sizeInterpolateFunction(
                startValue.sizeData, endValue.sizeData, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE():
              newVale.sizeData
                = sizeInterpolateFunction(startValue.sizeData,
                                          endValue.sizeData, gradient)
                    .add(offsetValue.sizeData.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Color3
        case Animation::ANIMATIONTYPE_COLOR3():
          switch (iLoopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE():
            case Animation::ANIMATIONLOOPMODE_CONSTANT():
              newVale.color3Data = color3InterpolateFunction(
                startValue.color3Data, endValue.color3Data, gradient);
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE():
              newVale.color3Data
                = color3InterpolateFunction(startValue.color3Data,
                                            endValue.color3Data, gradient)
                    .add(offsetValue.color3Data.scale(_repeatCount));
              return newVale;
            default:
              break;
          }
          break;
        // Matrix
        case Animation::ANIMATIONTYPE_MATRIX():
          switch (iLoopMode) {
            case Animation::ANIMATIONLOOPMODE_CYCLE():
            case Animation::ANIMATIONLOOPMODE_CONSTANT():
              if (Animation::AllowMatricesInterpolation()) {
                if (workValue) {
                  auto _workValue    = *workValue;
                  newVale.matrixData = matrixInterpolateFunction(
                    startValue.matrixData, endValue.matrixData, gradient,
                    _workValue.matrixData);
                  workValue = _workValue;
                }
                return newVale;
              }
              newVale.matrixData = startValue.matrixData;
              return newVale;
            case Animation::ANIMATIONLOOPMODE_RELATIVE():
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

Matrix Animation::matrixInterpolateFunction(Matrix& startValue,
                                            Matrix& endValue,
                                            float gradient) const
{
  if (Animation::AllowMatrixDecomposeForInterpolation()) {
    return Matrix::DecomposeLerp(startValue, endValue, gradient);
  }
  return Matrix::Lerp(startValue, endValue, gradient);
}

Matrix Animation::matrixInterpolateFunction(Matrix& startValue,
                                            Matrix& endValue, float gradient,
                                            Matrix& result) const
{
  if (Animation::AllowMatrixDecomposeForInterpolation()) {
    Matrix::DecomposeLerpToRef(startValue, endValue, gradient, result);
    return result;
  }

  Matrix::LerpToRef(startValue, endValue, gradient, result);
  return result;
}

AnimationPtr Animation::clone() const
{
  auto clonedAnimation
    = Animation::New(name, String::join(targetPropertyPath, '.'),
                     framePerSecond, dataType, loopMode);

  clonedAnimation->enableBlending = enableBlending;
  clonedAnimation->blendingSpeed  = blendingSpeed;

  if (!_keys.empty()) {
    clonedAnimation->setKeys(_keys);
  }

  if (!_ranges.empty()) {
    for (auto& range : _ranges) {
      clonedAnimation->_ranges[range.first] = range.second.copy();
    }
  }

  return clonedAnimation;
}

void Animation::setKeys(const std::vector<IAnimationKey>& values)
{
  _keys = values;
}

json Animation::serialize() const
{
  return nullptr;
}

AnimationPtr Animation::Parse(const json& parsedAnimation)
{
  auto animation = Animation::New(
    json_util::get_string(parsedAnimation, "name"),
    json_util::get_string(parsedAnimation, "property"),
    json_util::get_number(parsedAnimation, "framePerSecond", 30ull),
    json_util::get_number(parsedAnimation, "dataType", 0),
    json_util::get_number(parsedAnimation, "loopBehavior",
                          Animation::ANIMATIONLOOPMODE_CYCLE()));

  auto dataType = json_util::get_number(parsedAnimation, "dataType", 0u);
  std::vector<IAnimationKey> keys;

  if (json_util::has_key(parsedAnimation, "enableBlending")) {
    animation->enableBlending
      = json_util::get_bool(parsedAnimation, "enableBlending");
  }

  if (json_util::has_key(parsedAnimation, "blendingSpeed")) {
    animation->blendingSpeed
      = json_util::get_number<float>(parsedAnimation, "blendingSpeed");
  }

  for (auto& key : json_util::get_array<json>(parsedAnimation, "keys")) {
    std::optional<AnimationValue> inTangent  = std::nullopt;
    std::optional<AnimationValue> outTangent = std::nullopt;
    AnimationValue data;

    switch (dataType) {
      case Animation::ANIMATIONTYPE_FLOAT(): {
        auto values = json_util::get_array<float>(key, "values");
        data        = AnimationValue(values[0]);
        if (values.size() >= 1) {
          inTangent = values[1];
        }
        if (values.size() >= 2) {
          outTangent = values[2];
        }
      } break;
      case Animation::ANIMATIONTYPE_QUATERNION(): {
        auto values = json_util::get_array<float>(key, "values");
        data        = AnimationValue(Quaternion::FromArray(values));
        if (values.size() >= 8) {
          auto _inTangent
            = Quaternion::FromArray(stl_util::slice(values, 4, 8));
          if (!_inTangent.equals(Quaternion::Zero())) {
            inTangent = _inTangent;
          }
        }
        if (values.size() >= 12) {
          auto _outTangent
            = Quaternion::FromArray(stl_util::slice(values, 8, 12));
          if (!_outTangent.equals(Quaternion::Zero())) {
            outTangent = _outTangent;
          }
        }
      } break;
      case Animation::ANIMATIONTYPE_MATRIX():
        data = AnimationValue(
          Matrix::FromArray(json_util::get_array<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_COLOR3():
        data = AnimationValue(
          Color3::FromArray(json_util::get_array<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_VECTOR3():
      default:
        data = AnimationValue(
          Vector3::FromArray(json_util::get_array<float>(key, "values")));
        break;
    }

    IAnimationKey keyData(json_util::get_number(key, "frame", 0.f), data);
    if (inTangent.has_value()) {
      keyData.inTangent = *inTangent;
    }
    if (outTangent.has_value()) {
      keyData.outTangent = *outTangent;
    }

    keys.emplace_back(keyData);
  }

  animation->setKeys(keys);

  if (json_util::has_key(parsedAnimation, "ranges")) {
    for (const auto& data :
         json_util::get_array<json>(parsedAnimation, "ranges")) {
      animation->createRange(json_util::get_string(data, "name"),
                             json_util::get_number(data, "from", 0.f),
                             json_util::get_number(data, "to", 0.f));
    }
  }

  return animation;
}

void Animation::AppendSerializedAnimations(IAnimatable* /*source*/,
                                           json& /*destination*/)
{
}

} // end of namespace BABYLON
