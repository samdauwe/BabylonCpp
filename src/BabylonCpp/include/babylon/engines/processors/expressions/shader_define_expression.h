#ifndef BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_SHADER_DEFINE_EXPRESSION_H
#define BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_SHADER_DEFINE_EXPRESSION_H

#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT ShaderDefineExpression {

public:
  virtual ~ShaderDefineExpression() = default;

  virtual bool isTrue(const std::unordered_map<std::string, std::string>& preprocessors) const;

  static std::string postfixToInfix(const std::vector<std::string>& postfix);
  static std::vector<std::string> infixToPostfix(const std::string& infix);

private:
  static std::unordered_map<std::string, uint32_t> _OperatorPriority;
  static std::vector<std::string> _Stack;

}; // end of class ShaderDefineExpression

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_SHADER_DEFINE_EXPRESSION_H
