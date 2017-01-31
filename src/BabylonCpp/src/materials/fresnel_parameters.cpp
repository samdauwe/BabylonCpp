#include <babylon/materials/fresnel_parameters.h>

#include <babylon/core/json.h>

namespace BABYLON {

FresnelParameters::FresnelParameters()
    : isEnabled{true}
    , leftColor{Color3::White()}
    , rightColor{Color3::Black()}
    , bias{0.f}
    , power{1.f}
{
}

FresnelParameters::FresnelParameters(const FresnelParameters& other)
    : isEnabled{other.isEnabled}
    , leftColor{other.leftColor}
    , rightColor{other.rightColor}
    , bias{other.bias}
    , power{other.power}
{
}

FresnelParameters::~FresnelParameters()
{
}

std::unique_ptr<FresnelParameters> FresnelParameters::clone() const
{
  return std_util::make_unique<FresnelParameters>(*this);
}

Json::object FresnelParameters::serialize() const
{
  return Json::object();
}

std::unique_ptr<FresnelParameters>
FresnelParameters::Parse(const Json::value& parsedFresnelParameters)
{
  auto fresnelParameters = std_util::make_unique<FresnelParameters>();

  fresnelParameters->isEnabled
    = Json::GetBool(parsedFresnelParameters, "isEnabled", true);
  fresnelParameters->leftColor = Color3::FromArray(
    Json::ToArray<float>(parsedFresnelParameters, "leftColor"));
  fresnelParameters->rightColor = Color3::FromArray(
    Json::ToArray<float>(parsedFresnelParameters, "rightColor"));
  fresnelParameters->bias
    = Json::GetNumber<float>(parsedFresnelParameters, "bias", 0.f);
  fresnelParameters->power
    = Json::GetNumber<float>(parsedFresnelParameters, "power ", 1.f);

  return fresnelParameters;
}

} // end of namespace BABYLON
