#ifndef BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSING_OPTIONS_H
#define BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSING_OPTIONS_H

#include <nlohmann/json.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

using json = nlohmann::json;

namespace BABYLON {

FWD_STRUCT_SPTR(IShaderProcessor)
FWD_STRUCT_SPTR(ShaderProcessingContext)

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT ProcessingOptions {
  std::vector<std::string> defines{};
  json indexParameters;
  bool isFragment{false};
  bool shouldUseHighPrecisionShader{false};
  bool supportsUniformBuffers{false};
  std::string shadersRepository{""};
  std::unordered_map<std::string, std::string> includesShadersStore{};
  IShaderProcessorPtr processor{nullptr};
  std::string version{""};
  std::string platformName{""};
  std::optional<bool> lookForClosingBracketForUniformBuffer{std::nullopt};
  ShaderProcessingContextPtr processingContext{nullptr};
}; // end of struct ProcessingOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_SHADER_PROCESSING_OPTIONS_H
