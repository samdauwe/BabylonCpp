#ifndef BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H
#define BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/engines/ipipeline_context.h>

namespace BABYLON {

namespace GL {
class IGLProgram;
class IGLRenderingContext;
class IGLShader;
class IGLTransformFeedback;
} // end of namespace GL

class ThinEngine;
using WebGLProgramPtr           = std::shared_ptr<GL::IGLProgram>;
using WebGLRenderingContext     = GL::IGLRenderingContext;
using WebGLShaderPtr            = std::shared_ptr<GL::IGLShader>;
using WebGLTransformFeedbackPtr = std::shared_ptr<GL::IGLTransformFeedback>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGLPipelineContext : public IPipelineContext {

public:
  WebGLPipelineContext();
  ~WebGLPipelineContext() override; // = default

  /**
   * @brief Gets a boolean indicating that this pipeline context is supporting asynchronous
   * creating.
   */
  bool isAsync() override;

  /**
   * @brief Gets a boolean indicating that the context is ready to be used (like shaders / pipelines
   * are compiled and ready for instance).
   */
  bool isReady() override;

  void _handlesSpectorRebuildCallback(
    const std::function<void(const WebGLProgramPtr& program)>& onCompiled);

  /**
   * @brief Hidden
   */
  std::string _getVertexShaderCode() const override;

  /**
   * @brief Hidden
   */
  std::string _getFragmentShaderCode() const override;

public:
  ThinEngine* engine;
  WebGLProgramPtr program;
  WebGLRenderingContext* context;
  WebGLShaderPtr vertexShader;
  WebGLShaderPtr fragmentShader;
  bool isParallelCompiled;
  std::function<void()> onCompiled;
  WebGLTransformFeedbackPtr transformFeedback;

  std::string vertexCompilationError;
  std::string fragmentCompilationError;
  std::string programLinkError;
  std::string programValidationError;

}; // end of class WebGLPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H
