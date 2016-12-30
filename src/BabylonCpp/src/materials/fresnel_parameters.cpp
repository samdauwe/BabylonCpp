#include <babylon/materials/fresnel_parameters.h>

#include <babylon/core/json.h>

namespace BABYLON {

FresnelParameters::FresnelParameters()
    : isEnabled{true}
    , leftColor{Color3::White()}
    , rightColor{Color3::Black()}
    , bias{0}
    , power{1}
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
  return nullptr; // std_util::make_unique<FresnelParameters>(this);
}

Json::object FresnelParameters::serialize() const
{
  return Json::object();
}

std::unique_ptr<FresnelParameters>
FresnelParameters::Parse(const Json::value& /*source*/)
{
  return nullptr;
}

} // end of namespace BABYLON
