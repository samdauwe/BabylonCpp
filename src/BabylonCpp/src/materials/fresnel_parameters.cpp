#include <babylon/materials/fresnel_parameters.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/ifresnel_parameters_serialized.h>

namespace BABYLON {

FresnelParameters::FresnelParameters(
  const std::optional<IFresnelParametersCreationOptions>& options)
    : isEnabled{this, &FresnelParameters::get_isEnabled, &FresnelParameters::set_isEnabled}
{
  bias       = (!options || !options->bias) ? 0.f : *options->bias;
  power      = (!options || !options->power) ? 1.f : *options->power;
  leftColor  = (!options || !options->leftColor) ? Color3::White() : *options->leftColor;
  rightColor = (!options || !options->rightColor) ? Color3::Black() : *options->rightColor;
  isEnabled  = (!options || !options->isEnabled) ? true : *options->isEnabled;
}

FresnelParameters::FresnelParameters(const FresnelParameters& other)
    : isEnabled{this, &FresnelParameters::get_isEnabled, &FresnelParameters::set_isEnabled}
    , leftColor{other.leftColor}
    , rightColor{other.rightColor}
    , bias{other.bias}
    , power{other.power}
    , _isEnabled{other._isEnabled}
{
}

FresnelParameters::FresnelParameters(FresnelParameters&& other)
    : isEnabled{this, &FresnelParameters::get_isEnabled, &FresnelParameters::set_isEnabled}
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

bool FresnelParameters::equals(const FresnelParameters& otherFresnelParameters) const
{
  return (stl_util::almost_equal(bias, otherFresnelParameters.bias))
         && (stl_util::almost_equal(power, otherFresnelParameters.power))
         && (leftColor.equals(otherFresnelParameters.leftColor))
         && (rightColor.equals(otherFresnelParameters.rightColor))
         && (isEnabled == otherFresnelParameters.isEnabled);
}

IFresnelParametersSerialized FresnelParameters::serialize() const
{
  return {
    leftColor.asArray(),  // leftColor
    rightColor.asArray(), // rightColor
    bias,                 // bias
    power,                // power
    isEnabled             // isEnabled
  };
}

std::unique_ptr<FresnelParameters>
FresnelParameters::Parse(const IFresnelParametersSerialized& parsedFresnelParameters)
{
  return std::make_unique<FresnelParameters>(IFresnelParametersCreationOptions{
    Color3::FromArray(parsedFresnelParameters.leftColor),  // leftColor
    Color3::FromArray(parsedFresnelParameters.rightColor), // rightColor
    parsedFresnelParameters.bias,                          // bias
    parsedFresnelParameters.power.value_or(1.f),           // power
    parsedFresnelParameters.isEnabled,                     // isEnabled
  });
}

} // end of namespace BABYLON
