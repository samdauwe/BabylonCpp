#ifndef BABYLON_ENGINES_WEBGL_WEBGL_SHADER_PROCESSOR_H
#define BABYLON_ENGINES_WEBGL_WEBGL_SHADER_PROCESSOR_H

#include <babylon/babylon_api.h>
#include <babylon/engines/processors/ishader_processor.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT WebGLShaderProcessor : public IShaderProcessor {
  ~WebGLShaderProcessor() override; // = default
  std::string postProcessor(std::string code, const std::vector<std::string>& defines,
                            bool isFragment, ThinEngine* engine) override;
}; // end of struct WebGLShaderProcessor

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_WEBGL_WEBGL_SHADER_PROCESSOR_H
