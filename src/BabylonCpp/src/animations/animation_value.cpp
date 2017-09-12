#include <babylon/animations/animation_value.h>

#include <babylon/animations/animation.h>

namespace BABYLON {

AnimationValue::AnimationValue() : dataType{-1}
{
}

AnimationValue::AnimationValue(float value)
{
  if (!::std::isnan(value) && ::std::isfinite(value)) {
    dataType  = Animation::ANIMATIONTYPE_FLOAT;
    floatData = value;
  }
}

AnimationValue::AnimationValue(const Vector3& value)
    : dataType{Animation::ANIMATIONTYPE_VECTOR3}, vector3Data{value}
{
}

AnimationValue::AnimationValue(const Quaternion& value)
    : dataType{Animation::ANIMATIONTYPE_QUATERNION}, quaternionData{value}
{
}

AnimationValue::AnimationValue(const Matrix& value)
    : dataType{Animation::ANIMATIONTYPE_MATRIX}, matrixData{value}
{
}

AnimationValue::AnimationValue(const Color3& value)
    : dataType{Animation::ANIMATIONTYPE_COLOR3}, color3Data{value}
{
}

AnimationValue::AnimationValue(const Vector2& value)
    : dataType{Animation::ANIMATIONTYPE_VECTOR2}, vector2Data{value}
{
}

AnimationValue::AnimationValue(const Size& value)
    : dataType{Animation::ANIMATIONTYPE_SIZE}, sizeData{value}
{
}

AnimationValue::AnimationValue(bool value)
    : dataType{Animation::ANIMATIONTYPE_BOOL}, boolData{value}
{
}

AnimationValue::AnimationValue(int value)
    : dataType{Animation::ANIMATIONTYPE_INT}, intData{value}
{
}

AnimationValue::AnimationValue(const std::string& value)
    : dataType{Animation::ANIMATIONTYPE_STRING}, stringData{value}
{
}

AnimationValue::AnimationValue(const Color4& value)
    : dataType{Animation::ANIMATIONTYPE_COLOR4}, color4Data{value}
{
}

AnimationValue AnimationValue::subtract(const AnimationValue& fromValue)
{
  switch (dataType) {
    case Animation::ANIMATIONTYPE_FLOAT:
      return AnimationValue(floatData - fromValue.floatData);
    case Animation::ANIMATIONTYPE_VECTOR3:
      return AnimationValue(vector3Data.subtract(fromValue.vector3Data));
    case Animation::ANIMATIONTYPE_QUATERNION:
      return AnimationValue(quaternionData.subtract(fromValue.quaternionData));
    case Animation::ANIMATIONTYPE_MATRIX:
      return AnimationValue(matrixData.subtract(fromValue.matrixData));
    case Animation::ANIMATIONTYPE_COLOR3:
      return AnimationValue(color3Data.subtract(fromValue.color3Data));
    case Animation::ANIMATIONTYPE_VECTOR2:
      return AnimationValue(vector2Data.subtract(fromValue.vector2Data));
    case Animation::ANIMATIONTYPE_SIZE:
      return AnimationValue(sizeData.subtract(fromValue.sizeData));
    case Animation::ANIMATIONTYPE_INT:
      return AnimationValue(intData = fromValue.intData);
    case Animation::ANIMATIONTYPE_COLOR4:
      return AnimationValue(color4Data.subtract(fromValue.color4Data));
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
  if (dataType == Animation::ANIMATIONTYPE_BOOL) {
    return AnimationValue(!boolData);
  }
  return *this;
}

AnimationValue AnimationValue::operator[](const std::string& /*property*/)
{
  return AnimationValue(false);
}

AnimationValue AnimationValue::copy() const
{
  AnimationValue v;
  v.dataType = dataType;
  switch (dataType) {
    case Animation::ANIMATIONTYPE_FLOAT:
      v.floatData = floatData;
      break;
    case Animation::ANIMATIONTYPE_VECTOR3:
      v.vector3Data = vector3Data;
      break;
    case Animation::ANIMATIONTYPE_QUATERNION:
      v.quaternionData = quaternionData;
      break;
    case Animation::ANIMATIONTYPE_MATRIX:
      v.matrixData = matrixData;
      break;
    case Animation::ANIMATIONTYPE_COLOR3:
      v.color3Data = color3Data;
      break;
    case Animation::ANIMATIONTYPE_VECTOR2:
      v.vector2Data = vector2Data;
      break;
    case Animation::ANIMATIONTYPE_SIZE:
      v.sizeData = sizeData;
      break;
    case Animation::ANIMATIONTYPE_BOOL:
      v.boolData = boolData;
      break;
    case Animation::ANIMATIONTYPE_INT:
      v.intData = intData;
      break;
    case Animation::ANIMATIONTYPE_STRING:
      v.stringData = stringData;
      break;
    case Animation::ANIMATIONTYPE_COLOR4:
      v.color4Data = color4Data;
      break;
    default:
      break;
  }

  return v;
}

any AnimationValue::getValue() const
{
  any value = nullptr;
  switch (dataType) {
    case Animation::ANIMATIONTYPE_FLOAT:
      value = &floatData;
      break;
    case Animation::ANIMATIONTYPE_VECTOR3:
      value = &vector3Data;
      break;
    case Animation::ANIMATIONTYPE_QUATERNION:
      value = &quaternionData;
      break;
    case Animation::ANIMATIONTYPE_MATRIX:
      value = &matrixData;
      break;
    case Animation::ANIMATIONTYPE_COLOR3:
      value = &color3Data;
      break;
    case Animation::ANIMATIONTYPE_VECTOR2:
      value = &vector2Data;
      break;
    case Animation::ANIMATIONTYPE_SIZE:
      value = &sizeData;
      break;
    case Animation::ANIMATIONTYPE_BOOL:
      value = &boolData;
      break;
    case Animation::ANIMATIONTYPE_INT:
      value = &intData;
      break;
    case Animation::ANIMATIONTYPE_STRING:
      value = &stringData;
      break;
    case Animation::ANIMATIONTYPE_COLOR4:
      value = &color4Data;
      break;
    default:
      break;
  }

  return value;
}

} // end of namespace BABYLON
