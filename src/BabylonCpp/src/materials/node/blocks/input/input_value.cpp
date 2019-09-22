#include <babylon/materials/node/blocks/input/input_value.h>

namespace BABYLON {

InputValue::InputValue() : _value{std::nullopt}
{
}

InputValue::InputValue(const InputValue& other) : _value{other._value}
{
}

InputValue::InputValue(InputValue&& other) : _value{std::move(other._value)}
{
}

InputValue& InputValue::operator=(const InputValue& other)
{
  if (&other != this) {
    _value = other._value;
  }

  return *this;
}

InputValue& InputValue::operator=(InputValue&& other)
{
  if (&other != this) {
    _value = std::move(other._value);
  }

  return *this;
}

InputValue::~InputValue()
{
}

InputValue InputValue::copy() const
{
  return InputValue(*this);
}

std::optional<NodeMaterialBlockConnectionPointTypes> InputValue::getType() const
{
  if (!_value.has_value()) {
    return std::nullopt;
  }

  switch (_value->index()) {
    case 0:
      return NodeMaterialBlockConnectionPointTypes::Float;
    case 1:
      return NodeMaterialBlockConnectionPointTypes::Int;
    case 2:
      return NodeMaterialBlockConnectionPointTypes::Vector2;
    case 3:
      return NodeMaterialBlockConnectionPointTypes::Vector3;
    case 4:
      return NodeMaterialBlockConnectionPointTypes::Vector4;
    case 5:
      return NodeMaterialBlockConnectionPointTypes::Color3;
    case 6:
      return NodeMaterialBlockConnectionPointTypes::Color4;
    case 7:
      return NodeMaterialBlockConnectionPointTypes::Matrix;
    default:
      break;
  }

  return std::nullopt;
}

} // end of namespace BABYLON
