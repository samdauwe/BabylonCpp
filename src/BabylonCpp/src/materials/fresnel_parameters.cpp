#include <babylon/materials/fresnel_parameters.h>

#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/material.h>

namespace BABYLON {

FresnelParameters::FresnelParameters()
    : leftColor{Color3::White()}
    , rightColor{Color3::Black()}
    , bias{0.f}
    , power{1.f}
    , _isEnabled{true}
{
}

FresnelParameters::FresnelParameters(const FresnelParameters& other)
    : leftColor{other.leftColor}
    , rightColor{other.rightColor}
    , bias{other.bias}
    , power{other.power}
    , _isEnabled{other._isEnabled}
{
}

FresnelParameters::FresnelParameters(FresnelParameters&& other)
    : leftColor{std::move(other.leftColor)}
    , rightColor{std::move(other.rightColor)}
    , bias{std::move(other.bias)}
    , power{std::move(other.power)}
    , _isEnabled{std::move(other._isEnabled)}
{
}

FresnelParameters& FresnelParameters::operator=(const FresnelParameters& other)
{
  if (&other != this) {
    leftColor  = other.leftColor;
    rightColor = other.rightColor;
    bias       = other.bias;
    power      = other.power;
    _isEnabled = other._isEnabled;
  }

  return *this;
}

FresnelParameters& FresnelParameters::operator=(FresnelParameters&& other)
{
  if (&other != this) {
    leftColor  = std::move(other.leftColor);
    rightColor = std::move(other.rightColor);
    bias       = std::move(other.bias);
    power      = std::move(other.power);
    _isEnabled = std::move(other._isEnabled);
  }

  return *this;
}

FresnelParameters::~FresnelParameters()
{
}

bool FresnelParameters::isEnabled() const
{
  return _isEnabled;
}

void FresnelParameters::setIsEnabled(bool value)
{
  if (_isEnabled == value) {
    return;
  }

  _isEnabled = value;
  Engine::MarkAllMaterialsAsDirty(Material::FresnelDirtyFlag);
}

std::unique_ptr<FresnelParameters> FresnelParameters::clone() const
{
  return std::make_unique<FresnelParameters>(*this);
}

Json::object FresnelParameters::serialize() const
{
  return Json::object();
}

std::unique_ptr<FresnelParameters>
FresnelParameters::Parse(const Json::value& parsedFresnelParameters)
{
  auto fresnelParameters = std::make_unique<FresnelParameters>();

  fresnelParameters->setIsEnabled(
    Json::GetBool(parsedFresnelParameters, "isEnabled", true));
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
