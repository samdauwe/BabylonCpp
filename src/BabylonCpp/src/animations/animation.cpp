#include <babylon/animations/animation.h>

#include <babylon/animations/animatable.h>
#include <babylon/animations/easing/ieasing_function.h>
#include <babylon/animations/runtime_animation.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/json.h>
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

bool Animation::AllowMatricesInterpolation()
{
  return _AllowMatricesInterpolation;
}

Animation* Animation::_PrepareAnimation(
  const string_t& name, const string_t& targetProperty, size_t framePerSecond,
  int totalFrame, const AnimationValue& from, const AnimationValue& to,
  unsigned int loopMode, IEasingFunction* easingFunction)
{
  auto dataType = from.dataType;

  if (dataType == -1) {
    return nullptr;
  }

  auto animation
    = new Animation(name, targetProperty, framePerSecond, dataType, loopMode);

  animation->setKeys({
    AnimationKey(0, from), AnimationKey(totalFrame, to),
  });

  if (easingFunction != nullptr) {
    animation->setEasingFunction(easingFunction);
  }

  return animation;
}

Animation* Animation::CreateAnimation(const string_t& property,
                                      int animationType,
                                      std::size_t framePerSecond,
                                      IEasingFunction* easingFunction)
{
  auto animation
    = new Animation(property + "Animation", property, framePerSecond,
                    animationType, Animation::ANIMATIONLOOPMODE_CONSTANT);

  animation->setEasingFunction(easingFunction);

  return animation;
}

Animatable* Animation::CreateAndStartAnimation(
  const string_t& name, Node* node, const string_t& targetProperty,
  size_t framePerSecond, int totalFrame, const AnimationValue& from,
  const AnimationValue& to, unsigned int loopMode,
  IEasingFunction* easingFunction,
  const ::std::function<void()>& onAnimationEnd)
{

  auto animation = Animation::_PrepareAnimation(
    name, targetProperty, framePerSecond, totalFrame, from, to, loopMode,
    easingFunction);

  return node->getScene()->beginDirectAnimation(
    node, {animation}, 0, totalFrame, (animation->loopMode == 1), 1.f,
    onAnimationEnd);
}

Animatable* Animation::CreateMergeAndStartAnimation(
  const string_t& name, Node* node, const string_t& targetProperty,
  size_t framePerSecond, int totalFrame, const AnimationValue& from,
  const AnimationValue& to, unsigned int loopMode,
  IEasingFunction* easingFunction,
  const ::std::function<void()>& onAnimationEnd)
{
  auto animation = Animation::_PrepareAnimation(
    name, targetProperty, framePerSecond, totalFrame, from, to, loopMode,
    easingFunction);

  node->animations.emplace_back(animation);

  return node->getScene()->beginAnimation(
    node, 0, totalFrame, (animation->loopMode == 1), 1.f, onAnimationEnd);
}

Animatable* Animation::TransitionTo(
  const string_t& /*property*/, const AnimationValue& /*targetValue*/,
  const AnimationValue& /*host*/, Scene* /*scene*/, float /*frameRate*/,
  Animation* /*transition*/, float /*duration*/,
  const ::std::function<void()>& /*onAnimationEnd*/)
{
  return nullptr;
}

vector_t<RuntimeAnimation*>& Animation::runtimeAnimations()
{
  return _runtimeAnimations;
}

bool Animation::hasRunningRuntimeAnimations()
{
  for (auto& runtimeAnimation : _runtimeAnimations) {
    if (!runtimeAnimation->isStopped()) {
      return true;
    }
  }

  return false;
}

Animation::Animation(const string_t& iName, const string_t& iTargetProperty,
                     size_t iFramePerSecond, int iDataType,
                     unsigned int iLoopMode)

    : name{iName}
    , targetProperty{iTargetProperty}
    , targetPropertyPath{String::split(targetProperty, '.')}
    , framePerSecond{iFramePerSecond}
    , dataType{iDataType}
    , loopMode{iLoopMode}
    , blendingSpeed{0.01f}
    , enableBlending{false}
    , _easingFunction{nullptr}
{
}

Animation::~Animation()
{
}

string_t Animation::toString(bool fullDetails) const
{
  std::ostringstream oss;
  oss << "Name: " << name << ", property: " << targetProperty;
  if (dataType >= 0
      && static_cast<unsigned int>(dataType) <= ANIMATIONTYPE_BOOL) {
    size_t _dataType = static_cast<size_t>(dataType);
    oss << ", datatype: "
        << vector_t<string_t>{
             "Float",  "Vector3", "Quaternion", "Matrix",
             "Color3", "Vector2", "Size",       "Boolean"}[_dataType];
  }
  oss << ", nKeys: "
      << (!_keys.empty() ? ::std::to_string(_keys.size()) : "none");
  oss << ", nRanges: "
      << (!_ranges.empty() ? ::std::to_string(_ranges.size()) : "none");
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
  _events.erase(::std::remove_if(_events.begin(), _events.end(),
                                 [frame](const AnimationEvent& event) {
                                   return event.frame == frame;
                                 }),
                _events.end());
}

vector_t<AnimationEvent>& Animation::getEvents()
{
  return _events;
}

void Animation::createRange(const string_t& _name, float from, float to)
{
  // check name not already in use; could happen for bones after serialized
  if (!stl_util::contains(_ranges, _name)) {
    _ranges[name] = AnimationRange(_name, from, to);
  }
}

void Animation::deleteRange(const string_t& iName, bool deleteFrames)
{
  if (stl_util::contains(_ranges, iName)) {
    if (deleteFrames) {
      auto from = _ranges[iName].from;
      auto to   = _ranges[iName].to;

      _keys.erase(::std::remove_if(_keys.begin(), _keys.end(),
                                   [from, to](const AnimationKey& key) {
                                     return key.frame >= from
                                            && key.frame <= to;
                                   }),
                  _keys.end());
    }
    _ranges.erase(iName);
  }
}

AnimationRange& Animation::getRange(const string_t& iName)
{
  return _ranges[iName];
}

vector_t<AnimationKey>& Animation::getKeys()
{
  return _keys;
}

int Animation::getHighestFrame() const
{
  int ret = 0;
  for (auto& key : _keys) {
    if (ret < key.frame) {
      ret = key.frame;
    }
  }
  return ret;
}

IEasingFunction* Animation::getEasingFunction()
{
  return _easingFunction;
}

void Animation::setEasingFunction(IEasingFunction* easingFunction)
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

Matrix Animation::matrixInterpolateFunction(const Matrix& startValue,
                                            const Matrix& endValue,
                                            float gradient) const
{
  return Matrix::Lerp(startValue, endValue, gradient);
}

unique_ptr_t<Animation> Animation::clone() const
{
  auto clonedAnimation
    = ::std::make_unique<Animation>(name, String::join(targetPropertyPath, '.'),
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

void Animation::setKeys(const vector_t<AnimationKey>& values)
{
  _keys = values;
}

Json::object Animation::serialize() const
{
  auto serializationObject
    = Json::object({Json::Pair("name", name),                             //
                    Json::Pair("property", targetProperty),               //
                    Json::Pair<size_t>("framePerSecond", framePerSecond), //
                    Json::Pair<int>("dataType", dataType),                //
                    Json::Pair<unsigned>("loopBehavior", loopMode)});

  // Animation keys
  vector_t<Json::value> keys;
  for (auto& animationKey : _keys) {
    const AnimationValue& value = animationKey.value;
    Float32Array keyValues;
    switch (dataType) {
      case Animation::ANIMATIONTYPE_FLOAT:
        keyValues.emplace_back(value.floatData);
        break;
      case Animation::ANIMATIONTYPE_QUATERNION:
        stl_util::concat(keyValues, value.quaternionData.asArray());
        break;
      case Animation::ANIMATIONTYPE_MATRIX:
        stl_util::concat(keyValues, value.matrixData.asArray());
        break;
      case Animation::ANIMATIONTYPE_VECTOR3:
        stl_util::concat(keyValues, value.vector3Data.asArray());
        break;
      case Animation::ANIMATIONTYPE_COLOR3:
        stl_util::concat(keyValues, value.color3Data.asArray());
        break;
    }

    keys.emplace_back(
      Json::value(Json::object({Json::Pair<int>("frame", animationKey.frame), //
                                /*Json::Pair("values", keyValues)*/})));
  }
  serializationObject["keys"] = Json::value(keys);

  // Animation ranges
  vector_t<Json::value> ranges;
  for (auto& range : _ranges) {
    ranges.emplace_back(Json::value(Json::object(
      {Json::Pair("name", range.first),                              //
       Json::Pair<int>("from", static_cast<int>(range.second.from)), //
       Json::Pair<int>("to", static_cast<int>(range.second.to))})));
  }
  serializationObject["ranges"] = Json::value(ranges);

  return serializationObject;
}

Animation* Animation::Parse(const Json::value& parsedAnimation)
{
  auto animation
    = new Animation(Json::GetString(parsedAnimation, "name"),
                    Json::GetString(parsedAnimation, "property"),
                    Json::GetNumber(parsedAnimation, "framePerSecond", 30ull),
                    Json::GetNumber(parsedAnimation, "dataType", 0),
                    Json::GetNumber(parsedAnimation, "loopBehavior",
                                    Animation::ANIMATIONLOOPMODE_CYCLE));

  auto dataType = Json::GetNumber(parsedAnimation, "dataType", 0);
  vector_t<AnimationKey> keys;

  if (parsedAnimation.contains("enableBlending")) {
    animation->enableBlending
      = Json::GetBool(parsedAnimation, "enableBlending");
  }

  if (parsedAnimation.contains("blendingSpeed")) {
    animation->blendingSpeed
      = Json::GetNumber(parsedAnimation, "blendingSpeed", 0.01f);
  }

  for (auto& key : Json::GetArray(parsedAnimation, "keys")) {
    AnimationValue data;

    switch (dataType) {
      case Animation::ANIMATIONTYPE_FLOAT:
        data = AnimationValue(Json::ToArray<float>(key, "values")[0]);
        break;
      case Animation::ANIMATIONTYPE_QUATERNION:
        data = AnimationValue(
          Quaternion::FromArray(Json::ToArray<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_MATRIX:
        data = AnimationValue(
          Matrix::FromArray(Json::ToArray<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_COLOR3:
        data = AnimationValue(
          Color3::FromArray(Json::ToArray<float>(key, "values")));
        break;
      case Animation::ANIMATIONTYPE_VECTOR3:
      default:
        data = AnimationValue(
          Vector3::FromArray(Json::ToArray<float>(key, "values")));
        break;
    }

    keys.emplace_back(AnimationKey(Json::GetNumber(key, "frame", 0), data));
  }

  animation->setKeys(keys);

  if (parsedAnimation.contains("ranges")) {
    for (auto& range : Json::GetArray(parsedAnimation, "ranges")) {
      animation->createRange(Json::GetString(range, "name"),
                             Json::GetNumber<float>(range, "from", 0),
                             Json::GetNumber<float>(range, "to", 0));
    }
  }

  return animation;
}

void Animation::AppendSerializedAnimations(IAnimatable* source,
                                           picojson::object& destination)
{
  picojson::array animations;
  if (!source->getAnimations().empty()) {
    for (auto& animation : source->getAnimations()) {
      animations.push_back(picojson::value(animation->serialize()));
    }
  }
  destination["animations"] = picojson::value(animations);
}

} // end of namespace BABYLON
