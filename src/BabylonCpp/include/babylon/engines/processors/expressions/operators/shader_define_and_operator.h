#ifndef BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_AND_OPERATOR_H
#define BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_AND_OPERATOR_H

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/expressions/shader_define_expression.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ShaderDefineAndOperator : public ShaderDefineExpression {

  ShaderDefineExpression leftOperand;
  ShaderDefineExpression rightOperand;

  [[nodiscard]] bool
  isTrue(const std::unordered_map<std::string, std::string>& preprocessors) const override;

}; // end of struct ShaderDefineAndOperator

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_AND_OPERATOR_H
