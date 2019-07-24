#ifndef BABYLON_ENGINES_IPIPELINE_CONTEXT_H
#define BABYLON_ENGINES_IPIPELINE_CONTEXT_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to store and describe the pipeline context associated with
 * an effect.
 */
struct BABYLON_SHARED_EXPORT IPipelineContext {
  /**
   * Gets a boolean indicating that this pipeline context is supporting
   * asynchronous creating
   */
  bool isAsync = false;
  /**
   * Gets a boolean indicating that the context is ready to be used (like
   * shaders / pipelines are compiled and ready for instance)
   */
  bool isReady = false;
}; // end of struct IPipelineContext

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_IPIPELINE_CONTEXT_H}
