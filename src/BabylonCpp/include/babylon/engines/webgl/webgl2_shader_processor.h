#ifndef BABYLON_ENGINES_WEBGL_WEBGL2_SHADER_PROCESSOR_H
#define BABYLON_ENGINES_WEBGL_WEBGL2_SHADER_PROCESSOR_H

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/ishader_processor.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT WebGL2ShaderProcessor : public IShaderProcessor {
  ~WebGL2ShaderProcessor() override; // = default
  std::string attributeProcessor(const std::string& attribute) override;
  std::string varyingProcessor(const std::string& varying, bool isFragment) override;
  std::string postProcessor(std::string code, const std::vector<std::string>& defines,
                            bool isFragment) override;
}; // end of struct WebGL2ShaderProcessor

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_WEBGL_WEBGL2_SHADER_PROCESSOR_H
