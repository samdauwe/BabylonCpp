#ifndef BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_ARITHMETIC_OPERATOR_H
#define BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_ARITHMETIC_OPERATOR_H

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/expressions/shader_define_expression.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT ShaderDefineArithmeticOperator : public ShaderDefineExpression {

public:
  ShaderDefineArithmeticOperator();
  ShaderDefineArithmeticOperator(const std::string& define, const std::string& operand,
                                 const std::string& testValue);
  ShaderDefineArithmeticOperator(const ShaderDefineArithmeticOperator& other);
  ShaderDefineArithmeticOperator(ShaderDefineArithmeticOperator&& other);
  ShaderDefineArithmeticOperator& operator=(const ShaderDefineArithmeticOperator& other);
  ShaderDefineArithmeticOperator& operator=(ShaderDefineArithmeticOperator&& other);
  ~ShaderDefineArithmeticOperator() override; // = default

  [[nodiscard]] bool
  isTrue(const std::unordered_map<std::string, std::string>& preprocessors) const override;

public:
  std::string define;
  std::string operand;
  std::string testValue;

}; // end of class ShaderDefineArithmeticOperator

} // end of namespace BABYLON

#endif // end of
       // BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_ARITHMETIC_OPERATOR_H
