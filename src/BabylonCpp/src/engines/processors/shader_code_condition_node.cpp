#include <babylon/engines/processors/shader_code_condition_node.h>

namespace BABYLON {

std::string
ShaderCodeConditionNode::process(std::unordered_map<std::string, std::string>& preprocessors,
                                 ProcessingOptions& options)
{
  for (const auto& node : children) {
    if (node->isValid(preprocessors)) {
      return node->process(preprocessors, options);
    }
  }

  return "";
}

} // end of namespace BABYLON
