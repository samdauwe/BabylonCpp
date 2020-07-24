#ifndef BABYLON_RENDERING_PRE_PASS_EFFECT_CONFIGURATION_H
#define BABYLON_RENDERING_PRE_PASS_EFFECT_CONFIGURATION_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class PostProcess;
using PostProcessPtr = std::shared_ptr<PostProcess>;

/**
 * @brief Interface for defining prepass effects in the prepass post-process pipeline.
 */
struct BABYLON_SHARED_EXPORT PrePassEffectConfiguration {
  /**
   * Post process to attach for this effect
   */
  PostProcessPtr postProcess = nullptr;
  /**
   * Is the effect enabled
   */
  bool enabled = false;
  /**
   * @brief Disposes the effect configuration.
   */
  virtual void dispose() = 0;
  /**
   * @brief Creates the post-process.
   */
  virtual PostProcessPtr createPostProcess() = 0;
}; // end of struct PrePassEffectConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_PRE_PASS_EFFECT_CONFIGURATION_H
