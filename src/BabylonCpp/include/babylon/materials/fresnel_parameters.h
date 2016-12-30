#ifndef BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
#define BABYLON_MATERIALS_FRESNEL_PARAMETERS_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT FresnelParameters {

public:
  FresnelParameters();
  FresnelParameters(const FresnelParameters& other);
  ~FresnelParameters();

  std::unique_ptr<FresnelParameters> clone() const;
  Json::object serialize() const;

  static std::unique_ptr<FresnelParameters>
  Parse(const Json::value& source);

public:
  bool isEnabled;
  Color3 leftColor;
  Color3 rightColor;
  float bias;
  float power;

}; // end of class FresnelParameters

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_FRESNEL_PARAMETERS_H
