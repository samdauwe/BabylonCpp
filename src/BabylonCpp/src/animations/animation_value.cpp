#include <babylon/animations/animation_value.h>

#include <babylon/animations/animation.h>

namespace BABYLON {

AnimationValue::AnimationValue() : _value{std::nullopt}
{
}

AnimationValue::AnimationValue(const AnimationValue& other) = default;

AnimationValue::AnimationValue(AnimationValue&& other) = default;

AnimationValue& AnimationValue::operator=(const AnimationValue& other) = default;

AnimationValue& AnimationValue::operator=(AnimationValue&& other) = default;

AnimationValue::~AnimationValue() = default;

AnimationValue AnimationValue::subtract(const AnimationValue& fromValue)
{
  auto dataType = animationType();
  if (!dataType.has_value()) {
    return *this;
  }

  switch (dataType.value()) {
    case Animation::ANIMATIONTYPE_FLOAT:
      return AnimationValue(get<float>() - fromValue.get<float>());
    case Animation::ANIMATIONTYPE_VECTOR3:
      return AnimationValue(get<Vector3>().subtract(fromValue.get<Vector3>()));
    case Animation::ANIMATIONTYPE_QUATERNION:
      return AnimationValue(get<Quaternion>().subtract(fromValue.get<Quaternion>()));
    case Animation::ANIMATIONTYPE_MATRIX:
      return AnimationValue(get<Matrix>().subtract(fromValue.get<Matrix>()));
    case Animation::ANIMATIONTYPE_COLOR3:
      return AnimationValue(get<Color3>().subtract(fromValue.get<Color3>()));
    case Animation::ANIMATIONTYPE_VECTOR2:
      return AnimationValue(get<Vector2>().subtract(fromValue.get<Vector2>()));
    case Animation::ANIMATIONTYPE_SIZE:
      return AnimationValue(get<Size>().subtract(fromValue.get<Size>()));
    case Animation::ANIMATIONTYPE_INT:
      return AnimationValue(get<int>() = fromValue.get<int>());
    case Animation::ANIMATIONTYPE_COLOR4:
      return AnimationValue(get<Color4>().subtract(fromValue.get<Color4>()));
    case Animation::ANIMATIONTYPE_FLOAT32ARRAY: {
      const auto& currentArray = get<Float32Array>();
      const auto& otherArray   = fromValue.get<Float32Array>();
      auto count               = std::min(currentArray.size(), otherArray.size());
      Float32Array result(count);
      for (size_t i = 0; i < count; ++i) {
        result[i] = currentArray[i] - otherArray[i];
      }
      return AnimationValue(result);
    }
    case Animation::ANIMATIONTYPE_VECTOR4:
      return AnimationValue(get<Vector4>().subtract(fromValue.get<Vector4>()));
    default:
      return *this;
  }
}

AnimationValue AnimationValue::operator-(const AnimationValue& fromValue)
{
  return subtract(fromValue);
}

AnimationValue AnimationValue::operator!()
{
  if (_value.has_value() && animationType() == Animation::ANIMATIONTYPE_BOOL) {
    return AnimationValue(!std::get<bool>(_value.value()));
  }
  return *this;
}

AnimationValue AnimationValue::operator[](const std::string& /*property*/)
{
  return AnimationValue(false);
}

AnimationValue AnimationValue::copy() const
{
  return AnimationValue(*this);
}

std::optional<unsigned int> AnimationValue::animationType() const
{
  if (!_value.has_value()) {
    return std::nullopt;
  }

  switch (_value->index()) {
    case 0:
      return Animation::ANIMATIONTYPE_BOOL;
    case 1:
      return Animation::ANIMATIONTYPE_INT;
    case 2:
      return Animation::ANIMATIONTYPE_FLOAT;
    case 3:
      return Animation::ANIMATIONTYPE_STRING;
    case 4:
      return Animation::ANIMATIONTYPE_SIZE;
    case 5:
      return Animation::ANIMATIONTYPE_COLOR3;
    case 6:
      return Animation::ANIMATIONTYPE_COLOR4;
    case 7:
      return Animation::ANIMATIONTYPE_VECTOR2;
    case 8:
      return Animation::ANIMATIONTYPE_VECTOR3;
    case 9:
      return Animation::ANIMATIONTYPE_VECTOR4;
    case 10:
      return Animation::ANIMATIONTYPE_QUATERNION;
    case 11:
      return Animation::ANIMATIONTYPE_MATRIX;
    case 12:
      return Animation::ANIMATIONTYPE_FLOAT32ARRAY;
    default:
      break;
  }

  return std::nullopt;
}

} // end of namespace BABYLON
