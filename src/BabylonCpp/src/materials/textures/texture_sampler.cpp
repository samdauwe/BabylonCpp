#include <babylon/materials/textures/texture_sampler.h>

namespace BABYLON {

TextureSampler::TextureSampler()
    : samplingMode{-1}
    , wrapU{this, &TextureSampler::get_wrapU, &TextureSampler::set_wrapU}
    , wrapV{this, &TextureSampler::get_wrapV, &TextureSampler::set_wrapV}
    , wrapR{this, &TextureSampler::get_wrapR, &TextureSampler::set_wrapR}
    , anisotropicFilteringLevel{this, &TextureSampler::get_anisotropicFilteringLevel,
                                &TextureSampler::set_anisotropicFilteringLevel}
    , comparisonFunction{this, &TextureSampler::get_comparisonFunction,
                         &TextureSampler::set_comparisonFunction}
    , useMipMaps{this, &TextureSampler::get_useMipMaps, &TextureSampler::set_useMipMaps}
    , _cachedWrapU{std::nullopt}
    , _cachedWrapV{std::nullopt}
    , _cachedWrapR{std::nullopt}
    , _cachedAnisotropicFilteringLevel{std::nullopt}
    , _comparisonFunction{0}
    , _useMipMaps{true}
{
}

std::optional<uint32_t>& TextureSampler::get_wrapU()
{
  return _cachedWrapU;
}

void TextureSampler::set_wrapU(const std::optional<uint32_t>& value)
{
  _cachedWrapU = value;
}

std::optional<uint32_t>& TextureSampler::get_wrapV()
{
  return _cachedWrapV;
}

void TextureSampler::set_wrapV(const std::optional<uint32_t>& value)
{
  _cachedWrapV = value;
}

std::optional<uint32_t>& TextureSampler::get_wrapR()
{
  return _cachedWrapR;
}

void TextureSampler::set_wrapR(const std::optional<uint32_t>& value)
{
  _cachedWrapR = value;
}

std::optional<uint32_t>& TextureSampler::get_anisotropicFilteringLevel()
{
  return _cachedAnisotropicFilteringLevel;
}

void TextureSampler::set_anisotropicFilteringLevel(const std::optional<uint32_t>& value)
{
  _cachedAnisotropicFilteringLevel = value;
}

uint32_t TextureSampler::get_comparisonFunction() const
{
  return _comparisonFunction;
}

void TextureSampler::set_comparisonFunction(uint32_t value)
{
  _comparisonFunction = value;
}

bool TextureSampler::get_useMipMaps() const
{
  return _useMipMaps;
}

void TextureSampler::set_useMipMaps(bool value)
{
  _useMipMaps = value;
}

TextureSampler& TextureSampler::setParameters(uint32_t iWrapU, uint32_t iWrapV, uint32_t iWrapR,
                                              uint32_t iAnisotropicFilteringLevel,
                                              uint32_t iSamplingMode, uint32_t iComparisonFunction)
{
  _cachedWrapU                     = iWrapU;
  _cachedWrapV                     = iWrapV;
  _cachedWrapR                     = iWrapR;
  _cachedAnisotropicFilteringLevel = iAnisotropicFilteringLevel;
  samplingMode                     = static_cast<int>(iSamplingMode);
  _comparisonFunction              = iComparisonFunction;

  return *this;
}

bool TextureSampler::compareSampler(const TextureSampler& other) const
{
  return _cachedWrapU == other._cachedWrapU &&                                         //
         _cachedWrapV == other._cachedWrapV &&                                         //
         _cachedWrapR == other._cachedWrapR &&                                         //
         _cachedAnisotropicFilteringLevel == other._cachedAnisotropicFilteringLevel && //
         samplingMode == other.samplingMode &&                                         //
         _comparisonFunction == other._comparisonFunction &&                           //
         _useMipMaps == other._useMipMaps;
}

} // end of namespace BABYLON
