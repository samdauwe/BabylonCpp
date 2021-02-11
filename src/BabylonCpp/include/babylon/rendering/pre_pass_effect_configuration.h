#ifndef BABYLON_RENDERING_PRE_PASS_EFFECT_CONFIGURATION_H
#define BABYLON_RENDERING_PRE_PASS_EFFECT_CONFIGURATION_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

FWD_CLASS_SPTR(PostProcess)

/**
 * @brief Interface for defining prepass effects in the prepass post-process pipeline.
 */
struct BABYLON_SHARED_EXPORT PrePassEffectConfiguration {
  /**
   * Destructor
   */
  virtual ~PrePassEffectConfiguration() = default;
  /**
   * @brief Returns the name of the configuration.
   */
  virtual std::string name() const
  {
    return "";
  }
  /**
   * Post process to attach for this effect
   */
  PostProcessPtr postProcess = nullptr;
  /**
   * @brief Textures required in the MRT.
   */
  virtual std::vector<uint32_t> texturesRequired() const
  {
    return {};
  }
  /**
   * Is the effect enabled
   */
  bool enabled = false;
  /**
   * Does the output of this prepass need to go through imageprocessing
   */
  std::optional<bool> needsImageProcessing = std::nullopt;
  /**
   * @brief Disposes the effect configuration.
   */
  virtual void dispose()
  {
  }
  /**
   * @brief Creates the post-process.
   */
  virtual PostProcessPtr createPostProcess()
  {
    return nullptr;
  };
}; // end of struct PrePassEffectConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_PRE_PASS_EFFECT_CONFIGURATION_H
