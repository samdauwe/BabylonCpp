#ifndef BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_IS_DEFINED_OPERATOR_H
#define BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_IS_DEFINED_OPERATOR_H

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/expressions/shader_define_expression.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT ShaderDefineIsDefinedOperator : public ShaderDefineExpression {

public:
  ShaderDefineIsDefinedOperator();
  ShaderDefineIsDefinedOperator(const std::string& define, bool iNot = false);
  ShaderDefineIsDefinedOperator(const ShaderDefineIsDefinedOperator& other);
  ShaderDefineIsDefinedOperator(ShaderDefineIsDefinedOperator&& other);
  ShaderDefineIsDefinedOperator& operator=(const ShaderDefineIsDefinedOperator& other);
  ShaderDefineIsDefinedOperator& operator=(ShaderDefineIsDefinedOperator&& other);
  virtual ~ShaderDefineIsDefinedOperator(); // = default

  bool isTrue(const std::unordered_map<std::string, std::string>& preprocessors) const override;

public:
  std::string define;
  bool _not;

}; // end of class ShaderDefineIsDefinedOperator

} // end of namespace BABYLON

#endif // end of
       // BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_OPERATORS_SHADER_DEFINE_IS_DEFINED_OPERATOR_H
