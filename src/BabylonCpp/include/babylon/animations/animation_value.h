#ifndef BABYLON_ANIMATIONS_ANIMATION_VALUE_H
#define BABYLON_ANIMATIONS_ANIMATION_VALUE_H

#include <optional>
#include <string>
#include <variant>

#include <babylon/babylon_api.h>

#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/size.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

#ifndef isNan
#define isNan(a) (a != a)
#endif

namespace BABYLON {

class BABYLON_SHARED_EXPORT AnimationValue {

public:
  using AnimationValueType =
        std::variant<bool, int, float, long, void *, std::string, Size, Color3, Color4, Vector2,
                   Vector3, Quaternion, Matrix, Float32Array>;

public:
  AnimationValue();

  template <typename T,
            typename
            = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  AnimationValue(T value)
  {
    if (!isNan(value)) {
      _value = value;
    }
  }

  template <typename T,
            typename
            = typename std::enable_if<!std::is_arithmetic<T>::value, T>::type>
  AnimationValue(const T& value)
  {
    _value = value;
  }

  template <typename T>
  T& get()
  {
    return std::get<T>(_value.value());
  }

  template <typename T>
  const T& get() const
  {
    return std::get<T>(_value.value());
  }

  AnimationValue(const AnimationValue& other);
  AnimationValue(AnimationValue&& other);
  AnimationValue& operator=(const AnimationValue& other);
  AnimationValue& operator=(AnimationValue&& other);
  ~AnimationValue(); // = default

  AnimationValue subtract(const AnimationValue& fromValue);
  AnimationValue operator-(const AnimationValue& fromValue);
  AnimationValue operator!();
  AnimationValue operator[](const std::string& property);
  AnimationValue copy() const;

  operator bool() const
  {
    return _value.has_value();
  }

  std::optional<unsigned int> animationType() const;

private:
  std::optional<AnimationValueType> _value;

}; // end of class AnimationValue

} // end of namespace BABYLON

#endif // end of BABYLON_ANIMATIONS_ANIMATION_VALUE_H
