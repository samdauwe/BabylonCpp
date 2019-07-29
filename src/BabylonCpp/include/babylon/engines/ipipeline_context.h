#ifndef BABYLON_ENGINES_IPIPELINE_CONTEXT_H
#define BABYLON_ENGINES_IPIPELINE_CONTEXT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Class used to store and describe the pipeline context associated with
 * an effect.
 */
class BABYLON_SHARED_EXPORT IPipelineContext {

public:
  IPipelineContext();
  virtual ~IPipelineContext();

protected:
  virtual bool get_isAsync() const;
  virtual bool get_isReady() const;

public:
  /**
   * Gets a boolean indicating that this pipeline context is supporting
   * asynchronous creating
   */
  ReadOnlyProperty<IPipelineContext, bool> isAsync;
  /**
   * Gets a boolean indicating that the context is ready to be used (like
   * shaders / pipelines are compiled and ready for instance)
   */
  ReadOnlyProperty<IPipelineContext, bool> isReady;

private:
  bool _isAsync;
  bool _isReady;
}; // end of class IPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_IPIPELINE_CONTEXT_H}
