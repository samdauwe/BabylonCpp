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
using IGLProgramPtr           = std::shared_ptr<IGLProgram>;
using IGLShaderPtr            = std::shared_ptr<IGLShader>;
using IGLTransformFeedbackPtr = std::shared_ptr<IGLTransformFeedback>;
} // end of namespace GL

class Engine;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGLPipelineContext : public IPipelineContext {

public:
  WebGLPipelineContext();
  ~WebGLPipelineContext() override;

  void _handlesSpectorRebuildCallback(
    const std::function<void(const GL::IGLProgramPtr& program)>& onCompiled);

protected:
  bool get_isAsync() const override;
  bool get_isReady() const override;

public:
  Engine* engine;
  GL::IGLProgramPtr program;
  GL::IGLRenderingContext* context;
  GL::IGLShaderPtr vertexShader;
  GL::IGLShaderPtr fragmentShader;
  bool isParallelCompiled;
  std::function<void()> onCompiled;
  GL::IGLTransformFeedbackPtr transformFeedback;

}; // end of class WebGLPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H
