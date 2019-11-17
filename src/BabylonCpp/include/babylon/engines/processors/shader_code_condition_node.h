#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_CODE_CONDITION_NODE_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_CODE_CONDITION_NODE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/shader_code_node.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ShaderCodeConditionNode : public ShaderCodeNode {

  ~ShaderCodeConditionNode() override = default;

  std::string process(const std::unordered_map<std::string, std::string>& preprocessors,
                      ProcessingOptions& options) override;

}; // end of struct ShaderCodeConditionNode

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_SHADER_CODE_CONDITION_NODE_H
