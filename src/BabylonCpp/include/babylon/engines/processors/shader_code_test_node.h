#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_CODE_TEST_NODE_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_CODE_TEST_NODE_H

#include <string>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/expressions/shader_define_expression.h>
#include <babylon/engines/processors/shader_code_node.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ShaderCodeTestNode : public ShaderCodeNode {

  ShaderDefineExpression testExpression;

  virtual bool
  isValid(const std::unordered_map<std::string, std::string>& preprocessors) const override;

}; // end of struct ShaderCodeTestNode

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_SHADER_CODE_TEST_NODE_H
