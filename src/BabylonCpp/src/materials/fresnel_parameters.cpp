#include <babylon/materials/fresnel_parameters.h>

#include <nlohmann/json.hpp>

#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>

namespace BABYLON {

FresnelParameters::FresnelParameters()
    : isEnabled{this, &FresnelParameters::get_isEnabled,
                &FresnelParameters::set_isEnabled}
    , leftColor{Color3::White()}
    , rightColor{Color3::Black()}
    , bias{0.f}
    , power{1.f}
    , _isEnabled{true}
{
}

FresnelParameters::FresnelParameters(const FresnelParameters& other)
    : isEnabled{this, &FresnelParameters::get_isEnabled,
                &FresnelParameters::set_isEnabled}
    , leftColor{other.leftColor}
    , rightColor{other.rightColor}
    , bias{other.bias}
    , power{other.power}
    , _isEnabled{other._isEnabled}
{
}

FresnelParameters::FresnelParameters(FresnelParameters&& other)
    : isEnabled{this, &FresnelParameters::get_isEnabled,
                &FresnelParameters::set_isEnabled}
    , leftColor{std::move(other.leftColor)}
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

FresnelParameters::~FresnelParameters() = default;

bool FresnelParameters::get_isEnabled() const
{
  return _isEnabled;
}

void FresnelParameters::set_isEnabled(bool value)
{
  if (_isEnabled == value) {
    return;
  }

  _isEnabled = value;

  Engine::MarkAllMaterialsAsDirty(Constants::MATERIAL_FresnelDirtyFlag
                                  | Constants::MATERIAL_MiscDirtyFlag);
}

std::unique_ptr<FresnelParameters> FresnelParameters::clone() const
{
  return std::make_unique<FresnelParameters>(*this);
}

json FresnelParameters::serialize() const
{
  return nullptr;
}

std::unique_ptr<FresnelParameters>
FresnelParameters::Parse(const json& /*parsedFresnelParameters*/)
{
  return nullptr;
}

} // end of namespace BABYLON
