#ifndef BABYLON_RENDERING_SCREEN_SPACE_REFLECTIONS_CONFIGURATION_H
#define BABYLON_RENDERING_SCREEN_SPACE_REFLECTIONS_CONFIGURATION_H

#include <vector>

#include <babylon/engines/constants.h>
#include <babylon/rendering/pre_pass_effect_configuration.h>

namespace BABYLON {

/**
 * @brief Contains all parameters needed for the prepass to perform screen space reflections.
 */
struct BABYLON_SHARED_EXPORT ScreenSpaceReflectionsConfiguration
    : public PrePassEffectConfiguration {

  /**
   * Is ssr enabled
   */
  bool enabled = false;

  /**
   * @brief Returns the name of the configuration.
   */
  std::string name() const override
  {
    return "screenSpaceReflections";
  }

  /**
   * Textures that should be present in the MRT for this effect to work
   */
  std::vector<uint32_t> texturesRequired()
  {
    return {
      Constants::PREPASS_DEPTHNORMAL_TEXTURE_TYPE,
      Constants::PREPASS_REFLECTIVITY_TEXTURE_TYPE,
      Constants::PREPASS_POSITION_TEXTURE_TYPE,
    };
  };

}; // end of struct ScreenSpaceReflectionsConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_SCREEN_SPACE_REFLECTIONS_CONFIGURATION_H
