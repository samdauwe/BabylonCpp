#ifndef BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_EFFECT_BLUR_LEVEL_H
#define BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_EFFECT_BLUR_LEVEL_H

namespace BABYLON {

/**
 * @brief Specifies the level of max blur that should be applied when using the
 * depth of field effect
 */
enum class DepthOfFieldEffectBlurLevel {

  /**
   * Subtle blur
   */
  Low,
  /**
   * Medium blur
   */
  Medium,
  /**
   * Large blur
   */
  High

}; // end of enum class DepthOfFieldEffectBlurLevel

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_DEPTH_OF_FIELD_EFFECT_BLUR_LEVEL_H
