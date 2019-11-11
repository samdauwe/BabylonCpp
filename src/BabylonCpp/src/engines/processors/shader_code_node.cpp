#include <babylon/engines/processors/shader_code_node.h>

namespace BABYLON {

bool ShaderCodeNode::isValid(
  const std::unordered_map<std::string, std::string>& /*preprocessors*/) const
{
  return true;
}

std::string
ShaderCodeNode::process(const std::unordered_map<std::string, std::string>& /*preprocessors*/,
                        ProcessingOptions& /*options*/)
{
  return "";
}

} // end of namespace BABYLON
