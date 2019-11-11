#include <babylon/engines/processors/expressions/shader_define_expression.h>

namespace BABYLON {

bool ShaderDefineExpression::isTrue(
  const std::unordered_map<std::string, std::string>& /*preprocessors*/) const
{
  return true;
}

} // end of namespace BABYLON
