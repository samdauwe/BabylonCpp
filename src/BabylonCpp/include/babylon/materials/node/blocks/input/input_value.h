#ifndef BABYLON_MATERIALS_NODE_BLOCKS_INPUT_INPUT_VALUE_H
#define BABYLON_MATERIALS_NODE_BLOCKS_INPUT_INPUT_VALUE_H

#include <optional>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block_connection_point_types.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

#ifndef isNan
#define isNan(a) (a != a)
#endif

namespace BABYLON {

/**
 * @brief Represents an input value.
 */
class BABYLON_SHARED_EXPORT InputValue {

public:
  using InputValueType = std::variant<float, int, Vector2, Vector3, Vector4,
                                      Color3, Color4, Matrix>;

public:
  InputValue();

  template <typename T,
            typename
            = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  InputValue(T value)
  {
    if (!isNan(value)) {
      _value = value;
    }
  }

  template <typename T,
            typename
            = typename std::enable_if<!std::is_arithmetic<T>::value, T>::type>
  InputValue(const T& value)
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

  InputValue(const InputValue& other);
  InputValue(InputValue&& other);
  InputValue& operator=(const InputValue& other);
  InputValue& operator=(InputValue&& other);
  ~InputValue();

  InputValue copy() const;

  operator bool() const
  {
    return _value.has_value();
  }

  std::optional<NodeMaterialBlockConnectionPointTypes> getType() const;

private:
  std::optional<InputValueType> _value;

}; // end of class InputValue

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_INPUT_INPUT_VALUE_H
