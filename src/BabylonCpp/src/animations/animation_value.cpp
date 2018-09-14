#include <babylon/animations/animation_value.h>

#include <babylon/animations/animation.h>
#include <babylon/babylon_stl_util.h>

namespace BABYLON {

AnimationValue::AnimationValue() : dataType{-1}
{
}

AnimationValue::AnimationValue(float value)
{
  if (!isNan(value) && ::std::isfinite(value)) {
    dataType  = Animation::ANIMATIONTYPE_FLOAT();
    floatData = value;
  }
}

AnimationValue::AnimationValue(const Vector3& value)
    : dataType{(int)Animation::ANIMATIONTYPE_VECTOR3()}, vector3Data{value}
{
}

AnimationValue::AnimationValue(const Quaternion& value)
    : dataType{(int)Animation::ANIMATIONTYPE_QUATERNION()}, quaternionData{value}
{
}

AnimationValue::AnimationValue(const Matrix& value)
    : dataType{(int)Animation::ANIMATIONTYPE_MATRIX()}, matrixData{value}
{
}

AnimationValue::AnimationValue(const Color3& value)
    : dataType{(int)Animation::ANIMATIONTYPE_COLOR3()}, color3Data{value}
{
}

AnimationValue::AnimationValue(const Vector2& value)
    : dataType{(int)Animation::ANIMATIONTYPE_VECTOR2()}, vector2Data{value}
{
}

AnimationValue::AnimationValue(const Size& value)
    : dataType{(int)Animation::ANIMATIONTYPE_SIZE()}, sizeData{value}
{
}

AnimationValue::AnimationValue(bool value)
    : dataType{(int)Animation::ANIMATIONTYPE_BOOL()}, boolData{value}
{
}

AnimationValue::AnimationValue(int value)
    : dataType{(int)Animation::ANIMATIONTYPE_INT()}, intData{value}
{
}

AnimationValue::AnimationValue(const string_t& value)
    : dataType{(int)Animation::ANIMATIONTYPE_STRING()}, stringData{value}
{
}

AnimationValue::AnimationValue(const Color4& value)
    : dataType{(int)Animation::ANIMATIONTYPE_COLOR4()}, color4Data{value}
{
}

AnimationValue::AnimationValue(const Float32Array& value)
    : dataType{(int)Animation::ANIMATIONTYPE_FLOAT32ARRAY()}, float32ArrayData{value}
{
}

AnimationValue::AnimationValue(const AnimationValue& other)
    : dataType{other.dataType}
    , floatData{other.floatData}
    , vector3Data{other.vector3Data}
    , quaternionData{other.quaternionData}
    , matrixData{other.matrixData}
    , color3Data{other.color3Data}
    , vector2Data{other.vector2Data}
    , sizeData{other.sizeData}
    , boolData{other.boolData}
    , intData{other.intData}
    , stringData{other.stringData}
    , color4Data{other.color4Data}
    , float32ArrayData{other.float32ArrayData}
{
}

AnimationValue::AnimationValue(AnimationValue&& other)
    : dataType{::std::move(other.dataType)}
    , floatData{::std::move(other.floatData)}
    , vector3Data{::std::move(other.vector3Data)}
    , quaternionData{::std::move(other.quaternionData)}
    , matrixData{::std::move(other.matrixData)}
    , color3Data{::std::move(other.color3Data)}
    , vector2Data{::std::move(other.vector2Data)}
    , sizeData{::std::move(other.sizeData)}
    , boolData{::std::move(other.boolData)}
    , intData{::std::move(other.intData)}
    , stringData{::std::move(other.stringData)}
    , color4Data{::std::move(other.color4Data)}
    , float32ArrayData{::std::move(other.float32ArrayData)}
{
}

AnimationValue& AnimationValue::operator=(const AnimationValue& other)
{
  if (&other != this) {
    dataType         = other.dataType;
    floatData        = other.floatData;
    vector3Data      = other.vector3Data;
    quaternionData   = other.quaternionData;
    matrixData       = other.matrixData;
    color3Data       = other.color3Data;
    vector2Data      = other.vector2Data;
    sizeData         = other.sizeData;
    boolData         = other.boolData;
    intData          = other.intData;
    stringData       = other.stringData;
    color4Data       = other.color4Data;
    float32ArrayData = other.float32ArrayData;
  }

  return *this;
}

AnimationValue& AnimationValue::operator=(AnimationValue&& other)
{
  if (&other != this) {
    dataType         = ::std::move(other.dataType);
    floatData        = ::std::move(other.floatData);
    vector3Data      = ::std::move(other.vector3Data);
    quaternionData   = ::std::move(other.quaternionData);
    matrixData       = ::std::move(other.matrixData);
    color3Data       = ::std::move(other.color3Data);
    vector2Data      = ::std::move(other.vector2Data);
    sizeData         = ::std::move(other.sizeData);
    boolData         = ::std::move(other.boolData);
    intData          = ::std::move(other.intData);
    stringData       = ::std::move(other.stringData);
    color4Data       = ::std::move(other.color4Data);
    float32ArrayData = ::std::move(other.float32ArrayData);
  }

  return *this;
}

AnimationValue::~AnimationValue()
{
}

AnimationValue AnimationValue::subtract(const AnimationValue& fromValue)
{
  switch (dataType) {
    case Animation::ANIMATIONTYPE_FLOAT():
      return AnimationValue(floatData - fromValue.floatData);
    case Animation::ANIMATIONTYPE_VECTOR3():
      return AnimationValue(vector3Data.subtract(fromValue.vector3Data));
    case Animation::ANIMATIONTYPE_QUATERNION():
      return AnimationValue(quaternionData.subtract(fromValue.quaternionData));
    case Animation::ANIMATIONTYPE_MATRIX():
      return AnimationValue(matrixData.subtract(fromValue.matrixData));
    case Animation::ANIMATIONTYPE_COLOR3():
      return AnimationValue(color3Data.subtract(fromValue.color3Data));
    case Animation::ANIMATIONTYPE_VECTOR2():
      return AnimationValue(vector2Data.subtract(fromValue.vector2Data));
    case Animation::ANIMATIONTYPE_SIZE():
      return AnimationValue(sizeData.subtract(fromValue.sizeData));
    case Animation::ANIMATIONTYPE_INT():
      return AnimationValue(intData = fromValue.intData);
    case Animation::ANIMATIONTYPE_COLOR4():
      return AnimationValue(color4Data.subtract(fromValue.color4Data));
    case Animation::ANIMATIONTYPE_FLOAT32ARRAY(): {
      auto count = ::std::min(float32ArrayData.size(),
                              fromValue.float32ArrayData.size());
      Float32Array result(count);
      for (size_t i = 0; i < count; ++i) {
        result[i] = float32ArrayData[i] - fromValue.float32ArrayData[i];
      }
      return AnimationValue(result);
    }
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
  if (dataType == Animation::ANIMATIONTYPE_BOOL()) {
    return AnimationValue(!boolData);
  }
  return *this;
}

AnimationValue AnimationValue::operator[](const string_t& /*property*/)
{
  return AnimationValue(false);
}

AnimationValue AnimationValue::copy() const
{
  AnimationValue v;
  v.dataType = dataType;
  switch (dataType) {
    case Animation::ANIMATIONTYPE_FLOAT():
      v.floatData = floatData;
      break;
    case Animation::ANIMATIONTYPE_VECTOR3():
      v.vector3Data = vector3Data;
      break;
    case Animation::ANIMATIONTYPE_QUATERNION():
      v.quaternionData = quaternionData;
      break;
    case Animation::ANIMATIONTYPE_MATRIX():
      v.matrixData = matrixData;
      break;
    case Animation::ANIMATIONTYPE_COLOR3():
      v.color3Data = color3Data;
      break;
    case Animation::ANIMATIONTYPE_VECTOR2():
      v.vector2Data = vector2Data;
      break;
    case Animation::ANIMATIONTYPE_SIZE():
      v.sizeData = sizeData;
      break;
    case Animation::ANIMATIONTYPE_BOOL():
      v.boolData = boolData;
      break;
    case Animation::ANIMATIONTYPE_INT():
      v.intData = intData;
      break;
    case Animation::ANIMATIONTYPE_STRING():
      v.stringData = stringData;
      break;
    case Animation::ANIMATIONTYPE_COLOR4():
      v.color4Data = color4Data;
      break;
    case Animation::ANIMATIONTYPE_FLOAT32ARRAY():
      v.float32ArrayData = float32ArrayData;
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
    case Animation::ANIMATIONTYPE_FLOAT():
      value = &floatData;
      break;
    case Animation::ANIMATIONTYPE_VECTOR3():
      value = &vector3Data;
      break;
    case Animation::ANIMATIONTYPE_QUATERNION():
      value = &quaternionData;
      break;
    case Animation::ANIMATIONTYPE_MATRIX():
      value = &matrixData;
      break;
    case Animation::ANIMATIONTYPE_COLOR3():
      value = &color3Data;
      break;
    case Animation::ANIMATIONTYPE_VECTOR2():
      value = &vector2Data;
      break;
    case Animation::ANIMATIONTYPE_SIZE():
      value = &sizeData;
      break;
    case Animation::ANIMATIONTYPE_BOOL():
      value = &boolData;
      break;
    case Animation::ANIMATIONTYPE_INT():
      value = &intData;
      break;
    case Animation::ANIMATIONTYPE_STRING():
      value = &stringData;
      break;
    case Animation::ANIMATIONTYPE_COLOR4():
      value = &color4Data;
      break;
    case Animation::ANIMATIONTYPE_FLOAT32ARRAY():
      value = &float32ArrayData;
      break;
    default:
      break;
  }

  return value;
}

} // end of namespace BABYLON
