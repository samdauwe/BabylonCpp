#ifndef BABYLON_MATERIALS_IFRESNEL_PARAMETERS_SERIALIZED_H
#define BABYLON_MATERIALS_IFRESNEL_PARAMETERS_SERIALIZED_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Serialized format for FresnelParameters.
 */
struct BABYLON_SHARED_EXPORT IFresnelParametersSerialized {
  /**
   * Define the color used on edges (grazing angle) [as an array]
   */
  Float32Array leftColor;

  /**
   * Define the color used on center [as an array]
   */
  Float32Array rightColor;

  /**
   * Define bias applied to computed fresnel term
   */
  float bias;

  /**
   * Defined the power exponent applied to fresnel term
   */
  std::optional<float> power = std::nullopt;

  /**
   * Define if the fresnel effect is enable or not.
   */
  bool isEnabled;
}; // end of struct IFresnelParametersSerialized

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IFRESNEL_PARAMETERS_SERIALIZED_H
