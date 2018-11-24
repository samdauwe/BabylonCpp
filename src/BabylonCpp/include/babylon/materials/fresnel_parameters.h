#ifndef BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
#define BABYLON_MATERIALS_FRESNEL_PARAMETERS_H

#include <map>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>

using json = nlohmann::json;

namespace BABYLON {

class BABYLON_SHARED_EXPORT FresnelParameters {

public:
  FresnelParameters();
  FresnelParameters(const FresnelParameters& other);
  FresnelParameters(FresnelParameters&& other);
  FresnelParameters& operator=(const FresnelParameters& other);
  FresnelParameters& operator=(FresnelParameters&& other);
  ~FresnelParameters();

  std::unique_ptr<FresnelParameters> clone() const;
  json serialize() const;

  static std::unique_ptr<FresnelParameters>
  Parse(const json& parsedFresnelParameters);

private:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);

public:
  Property<FresnelParameters, bool> isEnabled;
  Color3 leftColor;
  Color3 rightColor;
  float bias;
  float power;

private:
  bool _isEnabled;

}; // end of class FresnelParameters

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
