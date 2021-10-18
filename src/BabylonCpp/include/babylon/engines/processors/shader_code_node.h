#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_CODE_NODE_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_CODE_NODE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

struct ProcessingOptions;
FWD_STRUCT_SPTR(ShaderCodeNode)

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ShaderCodeNode {
  virtual ~ShaderCodeNode() = default;
  std::string line;
  std::vector<ShaderCodeNodePtr> children;
  std::string additionalDefineKey;
  std::string additionalDefineValue;

  virtual bool isValid(const std::unordered_map<std::string, std::string>& preprocessors) const;
  virtual std::string process(std::unordered_map<std::string, std::string>& preprocessors,
                              ProcessingOptions& options);

}; // end of struct ShaderCodeNode

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_PROCESSING_OPTIONS_H
