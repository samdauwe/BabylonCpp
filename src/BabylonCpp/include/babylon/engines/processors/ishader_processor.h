#ifndef BABYLON_ENGINES_PROCESSORS_ISHADER_PROCESSOR_H
#define BABYLON_ENGINES_PROCESSORS_ISHADER_PROCESSOR_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class ThinEngine;
FWD_STRUCT_SPTR(ShaderProcessingContext)

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT IShaderProcessor {
  std::function<std::string(const std::string& attribute,
                            const std::unordered_map<std::string, std::string>& preProcessors,
                            const ShaderProcessingContextPtr& processingContext)>
    attributeProcessor = nullptr;
  std::function<std::string(const std::string& varying, bool isFragment,
                            const std::unordered_map<std::string, std::string>& preProcessors,
                            const ShaderProcessingContextPtr& processingContext)>
    varyingProcessor = nullptr;
  std::function<std::string(const std::string& uniform, bool isFragment,
                            const std::unordered_map<std::string, std::string>& preProcessors,
                            const ShaderProcessingContextPtr& processingContext)>
    uniformProcessor = nullptr;
  std::function<std::string(const std::string& uniformBuffer, bool isFragment,
                            const ShaderProcessingContextPtr& processingContext)>
    uniformBufferProcessor = nullptr;
  std::function<std::string(const std::string& closingBracketLine, bool isFragment,
                            const ShaderProcessingContextPtr& processingContext)>
    endOfUniformBufferProcessor = nullptr;

  std::function<std::string(const std::string& line, bool isFragment,
                            const ShaderProcessingContextPtr& processingContext)>
    lineProcessor = nullptr;
  std::function<std::string(const std::string& code, const std::vector<std::string>& defines,
                            bool isFragment, const ShaderProcessingContextPtr& processingContext)>
    preProcessor = nullptr;
  std::function<std::string(const std::string& code, const std::vector<std::string>& defines,
                            bool isFragment, const ShaderProcessingContextPtr& processingContext,
                            ThinEngine* engine)>
    postProcessor = nullptr;
  std::function<void(const ShaderProcessingContextPtr& processingContext)> initializeShaders
    = nullptr;
  std::function<std::unordered_map<std::string, std::string>(
    const std::string& vertexCode, const std::string& fragmentCode,
    const ShaderProcessingContextPtr& processingContext)>
    finalizeShaders = nullptr;
}; // end of struct IShaderProcessor

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_ISHADER_PROCESSOR_H
