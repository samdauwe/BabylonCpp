#ifndef BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_OR_OPERATOR_H
#define BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_OR_OPERATOR_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/expressions/shader_define_expression.h>

namespace BABYLON {

using ShaderDefineExpressionPtr = std::shared_ptr<ShaderDefineExpression>;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ShaderDefineOrOperator : public ShaderDefineExpression {

  ShaderDefineExpressionPtr leftOperand  = nullptr;
  ShaderDefineExpressionPtr rightOperand = nullptr;

  [[nodiscard]] bool
  isTrue(const std::unordered_map<std::string, std::string>& preprocessors) const override;

}; // end of struct ShaderDefineOrOperator

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_OR_OPERATOR_H
