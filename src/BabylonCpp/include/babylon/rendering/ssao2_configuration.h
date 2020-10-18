#ifndef BABYLON_RENDERING_SSAO2_CONFIGURATION_H
#define BABYLON_RENDERING_SSAO2_CONFIGURATION_H

#include <vector>

#include <babylon/engines/constants.h>
#include <babylon/rendering/pre_pass_effect_configuration.h>

namespace BABYLON {

/**
 * @brief Contains all parameters needed for the prepass to perform screen space subsurface
 * scattering.
 */
struct BABYLON_SHARED_EXPORT SSAO2Configuration : public PrePassEffectConfiguration {
  /**
   * Is subsurface enabled
   */
  bool enabled = false;

  /**
   * @brief Returns the name of the configuration.
   */
  std::string name() const override
  {
    return "ssao2";
  }

  /**
   * Textures that should be present in the MRT for this effect to work
   */
  std::vector<uint32_t> texturesRequired()
  {
    return {Constants::PREPASS_DEPTHNORMAL_TEXTURE_TYPE};
  };

}; // end of struct SSAO2Configuration

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_SSAO2_CONFIGURATION_H
