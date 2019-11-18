#ifndef BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_SHADER_DEFINE_EXPRESSION_H
#define BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_SHADER_DEFINE_EXPRESSION_H

#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ShaderDefineExpression {
  virtual ~ShaderDefineExpression() = default;

  [[nodiscard]] virtual bool
  isTrue(const std::unordered_map<std::string, std::string>& preprocessors) const;

}; // end of struct ShaderDefineExpression

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_EXPRESSIONS_SHADER_DEFINE_EXPRESSION_H
