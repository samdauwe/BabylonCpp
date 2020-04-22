#ifndef BABYLON_MATERIALS_IFRESNEL_PARAMETER_CREATION_OPTIONS_H
#define BABYLON_MATERIALS_IFRESNEL_PARAMETER_CREATION_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

/**
 * @brief Options to be used when creating a FresnelParameters.
 */
struct BABYLON_SHARED_EXPORT IFresnelParametersCreationOptions {
  /**
   * Define the color used on edges (grazing angle)
   */
  std::optional<Color3> leftColor = std::nullopt;

  /**
   * Define the color used on center
   */
  std::optional<Color3> rightColor = std::nullopt;

  /**
   * Define bias applied to computed fresnel term
   */
  std::optional<float> bias = std::nullopt;

  /**
   * Defined the power exponent applied to fresnel term
   */
  std::optional<float> power = std::nullopt;

  /**
   * Define if the fresnel effect is enable or not.
   */
  std::optional<bool> isEnabled = std::nullopt;
}; // end of struct IFresnelParametersCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IFRESNEL_PARAMETER_CREATION_OPTIONS_H
