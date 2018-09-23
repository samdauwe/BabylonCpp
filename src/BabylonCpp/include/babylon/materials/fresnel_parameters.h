#ifndef BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
#define BABYLON_MATERIALS_FRESNEL_PARAMETERS_H

#include <map>

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

class BABYLON_SHARED_EXPORT FresnelParameters {

public:
  FresnelParameters();
  FresnelParameters(const FresnelParameters& other);
  FresnelParameters(FresnelParameters&& other);
  FresnelParameters& operator=(const FresnelParameters& other);
  FresnelParameters& operator=(FresnelParameters&& other);
  ~FresnelParameters();

  std::unique_ptr<FresnelParameters> clone() const;
  Json::object serialize() const;

  static std::unique_ptr<FresnelParameters>
  Parse(const Json::value& parsedFresnelParameters);

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
