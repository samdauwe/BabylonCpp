#include <babylon/engines/processors/expressions/operators/shader_define_arithmetic_operator.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

ShaderDefineArithmeticOperator::ShaderDefineArithmeticOperator()
    : define{""}, operand{""}, testValue{""}
{
}

ShaderDefineArithmeticOperator::ShaderDefineArithmeticOperator(const std::string& iDefine,
                                                               const std::string& iOperand,
                                                               const std::string& iTestValue)
    : define{iDefine}, operand{iOperand}, testValue{iTestValue}
{
}

ShaderDefineArithmeticOperator::ShaderDefineArithmeticOperator(
  const ShaderDefineArithmeticOperator& other)
    : define{other.define}, operand{other.operand}, testValue{other.testValue}
{
}

ShaderDefineArithmeticOperator::ShaderDefineArithmeticOperator(
  ShaderDefineArithmeticOperator&& other)
    : define{std::move(other.define)}
    , operand{std::move(other.operand)}
    , testValue{std::move(other.testValue)}
{
}

ShaderDefineArithmeticOperator&
ShaderDefineArithmeticOperator::operator=(const ShaderDefineArithmeticOperator& other)
{
  if (&other != this) {
    define    = other.define;
    operand   = other.operand;
    testValue = other.testValue;
  }

  return *this;
}

ShaderDefineArithmeticOperator&
ShaderDefineArithmeticOperator::operator=(ShaderDefineArithmeticOperator&& other)
{
  if (&other != this) {
    define    = std::move(other.define);
    operand   = std::move(other.operand);
    testValue = std::move(other.testValue);
  }

  return *this;
}

ShaderDefineArithmeticOperator::~ShaderDefineArithmeticOperator() = default;

bool ShaderDefineArithmeticOperator::isTrue(
  const std::unordered_map<std::string, std::string>& preprocessors) const
{
  auto value = !stl_util::contains(preprocessors, define) ? define : preprocessors.at(define);

  auto condition   = false;
  const auto left  = StringTools::toNumber<float>(value);
  const auto right = StringTools::toNumber<float>(testValue);

  if (operand == ">") {
    condition = left > right;
  }
  else if (operand == "<") {
    condition = left < right;
  }
  else if (operand == "<=") {
    condition = left <= right;
  }
  else if (operand == ">=") {
    condition = left >= right;
  }
  else if (operand == "==") {
    condition = stl_util::almost_equal(left, right);
  }

  return condition;
}

} // end of namespace BABYLON
