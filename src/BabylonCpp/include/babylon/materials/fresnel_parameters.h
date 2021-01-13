#ifndef BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
#define BABYLON_MATERIALS_FRESNEL_PARAMETERS_H

#include <babylon/babylon_api.h>
#include <babylon/materials/ifresnel_parameters_creation_options.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

struct IFresnelParametersSerialized;

/**
 * @brief This represents all the required information to add a fresnel effect on a material:
 * @see https://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
 */
class BABYLON_SHARED_EXPORT FresnelParameters {

public:
  /**
   * @brief Creates a new FresnelParameters object.
   *
   * @param options provide your own settings to optionally to override defaults
   */
  FresnelParameters(const std::optional<IFresnelParametersCreationOptions>& options = std::nullopt);
  FresnelParameters(const FresnelParameters& other);
  FresnelParameters(FresnelParameters&& other);
  FresnelParameters& operator=(const FresnelParameters& other);
  FresnelParameters& operator=(FresnelParameters&& other);
  ~FresnelParameters(); // = default

  /**
   * @brief Clones the current fresnel and its values.
   * @returns a clone fresnel configuration
   */
  std::unique_ptr<FresnelParameters> clone() const;

  /**
   * @brief Determines equality between FresnelParameters objects.
   * @param otherFresnelParameters defines the second operand
   * @returns true if the power, bias, leftColor, rightColor and isEnabled values are equal to the
   * given ones
   */
  bool equals(const FresnelParameters& otherFresnelParameters) const;

  /**
   * @brief Serializes the current fresnel parameters to a JSON representation.
   * @return the JSON serialization
   */
  IFresnelParametersSerialized serialize() const;

  /**
   * @brief Parse a JSON object and deserialize it to a new Fresnel parameter object.
   * @param parsedFresnelParameters Define the JSON representation
   * @returns the parsed parameters
   */
  static std::unique_ptr<FresnelParameters>
  Parse(const IFresnelParametersSerialized& parsedFresnelParameters);

protected:
  /**
   * @brief Define if the fresnel effect is enable or not.
   */
  bool get_isEnabled() const;
  void set_isEnabled(bool value);

public:
  /**
   * Define if the fresnel effect is enable or not
   */
  Property<FresnelParameters, bool> isEnabled;

  /**
   * Define the color used on edges (grazing angle)
   */
  Color3 leftColor;

  /**
   * Define the color used on center
   */
  Color3 rightColor;

  /**
   * Define bias applied to computed fresnel term
   */
  float bias;

  /**
   * Defined the power exponent applied to fresnel term
   */
  float power;

private:
  bool _isEnabled;

}; // end of class FresnelParameters

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
