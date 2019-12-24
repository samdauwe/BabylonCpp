#include <babylon/engines/webgl/webgl_pipeline_context.h>

#include <babylon/engines/engine.h>

namespace BABYLON {

WebGLPipelineContext::WebGLPipelineContext()
    : IPipelineContext{}
    , engine{nullptr}
    , program{nullptr}
    , context{nullptr}
    , vertexShader{nullptr}
    , fragmentShader{nullptr}
    , isParallelCompiled{false}
    , onCompiled{nullptr}
    , transformFeedback{nullptr}
{
}

WebGLPipelineContext::~WebGLPipelineContext() = default;

bool WebGLPipelineContext::isAsync()
{
  return isParallelCompiled;
}

bool WebGLPipelineContext::isReady()
{
  if (program) {
    if (isParallelCompiled) {
      return engine->_isRenderingStateCompiled(this);
    }
    return true;
  }

  return false;
}

void WebGLPipelineContext::_handlesSpectorRebuildCallback(
  const std::function<void(const GL::IGLProgramPtr& program)>& iOnCompiled)
{
  if (iOnCompiled && program) {
    iOnCompiled(program);
  }
}

} // end of namespace BABYLON
