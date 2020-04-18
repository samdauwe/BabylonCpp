#ifndef BABYLON_ENGINES_IPIPELINE_CONTEXT_H
#define BABYLON_ENGINES_IPIPELINE_CONTEXT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class IPipelineContext;
using IPipelineContextPtr = std::shared_ptr<IPipelineContext>;

/**
 * @brief Class used to store and describe the pipeline context associated with an effect.
 */
class BABYLON_SHARED_EXPORT IPipelineContext {

public:
  IPipelineContext();
  virtual ~IPipelineContext(); // = default

public:
  /**
   * Gets a boolean indicating that this pipeline context is supporting asynchronous creating.
   */
  virtual bool isAsync();

  /**
   * Gets a boolean indicating that the context is ready to be used (like shaders / pipelines are
   * compiled and ready for instance).
   */
  virtual bool isReady();

  /**
   * @brief Hidden
   */
  virtual std::string _getVertexShaderCode() const = 0;

  /**
   * @brief Hidden
   */
  virtual std::string _getFragmentShaderCode() const = 0;

  /**
   * Hidden
   */
  std::function<void(
    const std::function<void(const IPipelineContextPtr& pipelineContext)>& onCompiled)>
    _handlesSpectorRebuildCallback;

private:
  bool _isAsync;
  bool _isReady;
}; // end of class IPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_IPIPELINE_CONTEXT_H}
