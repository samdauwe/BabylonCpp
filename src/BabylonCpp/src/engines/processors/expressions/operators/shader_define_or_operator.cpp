#include <babylon/engines/processors/expressions/operators/shader_define_or_operator.h>

namespace BABYLON {

bool ShaderDefineOrOperator::isTrue(
  const std::unordered_map<std::string, std::string>& preprocessors) const
{
  return leftOperand.isTrue(preprocessors) || rightOperand.isTrue(preprocessors);
}

} // end of namespace BABYLON
