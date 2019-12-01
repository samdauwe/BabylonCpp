#include <babylon/engines/processors/shader_code_test_node.h>

namespace BABYLON {

bool ShaderCodeTestNode::isValid(
  const std::unordered_map<std::string, std::string>& preprocessors) const
{
  return testExpression->isTrue(preprocessors);
}

} // end of namespace BABYLON
