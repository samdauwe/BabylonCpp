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

class Engine;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT WebGLPipelineContext : public IPipelineContext {

public:
  WebGLPipelineContext();
  ~WebGLPipelineContext() override;

  void _handlesSpectorRebuildCallback(
    const std::function<void(GL::IGLProgram* program)>& onCompiled);

protected:
  bool get_isAsync() const override;
  bool get_isReady() const override;

public:
  Engine* engine;
  GL::IGLProgram* program;
  GL::IGLRenderingContext* context;
  GL::IGLShader* vertexShader;
  GL::IGLShader* fragmentShader;
  bool isParallelCompiled;
  std::function<void()> onCompiled;
  GL::IGLTransformFeedback* transformFeedback;

}; // end of class WebGLPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_WEBGL_WEBGL_PIPELINE_CONTEXT_H
