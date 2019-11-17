#ifndef BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
#define BABYLON_MATERIALS_FRESNEL_PARAMETERS_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>

using json = nlohmann::json;

namespace BABYLON {

/**
 * @brief This represents all the required information to add a fresnel effect
 * on a material:
 * @see http://doc.babylonjs.com/how_to/how_to_use_fresnelparameters
 */
class BABYLON_SHARED_EXPORT FresnelParameters {

public:
  FresnelParameters();
  FresnelParameters(const FresnelParameters& other);
  FresnelParameters(FresnelParameters&& other);
  FresnelParameters& operator=(const FresnelParameters& other);
  FresnelParameters& operator=(FresnelParameters&& other);
  ~FresnelParameters(); // = default

  /**
   * @brief Clones the current fresnel and its values.
   * @returns a clone fresnel configuration
   */
  [[nodiscard]] std::unique_ptr<FresnelParameters> clone() const;

  /**
   * @brief Serializes the current fresnel parameters to a JSON representation.
   * @return the JSON serialization
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Parse a JSON object and deserialize it to a new Fresnel parameter
   * object.
   * @param parsedFresnelParameters Define the JSON representation
   * @returns the parsed parameters
   */
  static std::unique_ptr<FresnelParameters> Parse(const json& parsedFresnelParameters);

protected:
  /**
   * @brief Define if the fresnel effect is enable or not.
   */
  [[nodiscard]] bool get_isEnabled() const;
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
