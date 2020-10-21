#ifndef BABYLON_ENGINES_PROCESSORS_ISHADER_PROCESSOR_H
#define BABYLON_ENGINES_PROCESSORS_ISHADER_PROCESSOR_H

#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class ThinEngine;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT IShaderProcessor {
  virtual ~IShaderProcessor(); // = default
  virtual std::string attributeProcessor(const std::string& attribute);
  virtual std::string varyingProcessor(const std::string& varying, bool isFragment);
  virtual std::string uniformProcessor(const std::string& uniform, bool isFragment);
  virtual std::string uniformBufferProcessor(const std::string& uniformBuffer, bool isFragment);
  virtual std::string endOfUniformBufferProcessor(const std::string& closingBracketLine,
                                                  bool isFragment);
  virtual std::string lineProcessor(const std::string& line, bool isFragment);
  virtual std::string preProcessor(const std::string& code, const std::vector<std::string>& defines,
                                   bool isFragment);
  virtual std::string postProcessor(std::string code, const std::vector<std::string>& defines,
                                    bool isFragment, ThinEngine* engine);
}; // end of struct IShaderProcessor

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_PROCESSORS_ISHADER_PROCESSOR_H
