#ifndef BABYLON_MATERIALS_TEXTURES_TEXTURE_SAMPLER_H
#define BABYLON_MATERIALS_TEXTURES_TEXTURE_SAMPLER_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/engines/constants.h>

namespace BABYLON {

/**
 * @brief Class used to store a texture sampler data.
 */
class BABYLON_SHARED_EXPORT TextureSampler {

public:
  /**
   * @brief Creates a Sampler instance.
   */
  TextureSampler();
  ~TextureSampler() = default;

  /**
   * @brief Sets all the parameters of the sampler.
   * @param wrapU u address mode (default: TEXTURE_WRAP_ADDRESSMODE)
   * @param wrapV v address mode (default: TEXTURE_WRAP_ADDRESSMODE)
   * @param wrapR r address mode (default: TEXTURE_WRAP_ADDRESSMODE)
   * @param anisotropicFilteringLevel anisotropic level (default: 1)
   * @param samplingMode sampling mode (default: Constants.TEXTURE_BILINEAR_SAMPLINGMODE)
   * @param comparisonFunction comparison function (default: 0 - no comparison function)
   * @returns the current sampler instance
   */
  TextureSampler& setParameters(uint32_t wrapU = Constants::TEXTURE_WRAP_ADDRESSMODE,
                                uint32_t wrapV = Constants::TEXTURE_WRAP_ADDRESSMODE,
                                uint32_t wrapR = Constants::TEXTURE_WRAP_ADDRESSMODE,
                                uint32_t anisotropicFilteringLevel = 1,
                                uint32_t samplingMode = Constants::TEXTURE_BILINEAR_SAMPLINGMODE,
                                uint32_t comparisonFunction = 0);

  /**
   * @brief Compares this sampler with another one.
   * @param other sampler to compare with
   * @returns true if the samplers have the same parametres, else false
   */
  bool compareSampler(const TextureSampler& other) const;

protected:
  std::optional<uint32_t>& get_wrapU();
  void set_wrapU(const std::optional<uint32_t>& value);
  std::optional<uint32_t>& get_wrapV();
  void set_wrapV(const std::optional<uint32_t>& value);
  std::optional<uint32_t>& get_wrapR();
  void set_wrapR(const std::optional<uint32_t>& value);
  std::optional<uint32_t>& get_anisotropicFilteringLevel();
  void set_anisotropicFilteringLevel(const std::optional<uint32_t>& value);
  uint32_t get_comparisonFunction() const;
  void set_comparisonFunction(uint32_t value);
  bool get_useMipMaps() const;
  void set_useMipMaps(bool value);

public:
  /**
   * Gets the sampling mode of the texture
   */
  int samplingMode;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  Property<TextureSampler, std::optional<uint32_t>> wrapU;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  Property<TextureSampler, std::optional<uint32_t>> wrapV;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  Property<TextureSampler, std::optional<uint32_t>> wrapR;

  /**
   * With compliant hardware and browser (supporting anisotropic filtering)
   * this defines the level of anisotropic filtering in the texture.
   * The higher the better but the slower.
   */
  Property<TextureSampler, std::optional<uint32_t>> anisotropicFilteringLevel;

  /**
   * Gets or sets the comparison function (Constants.LESS, Constants.EQUAL, etc). Set 0 to not use a
   * comparison function
   */
  Property<TextureSampler, uint32_t> comparisonFunction;

  /**
   * Indicates to use the mip maps (if available on the texture).
   * Thanks to this flag, you can instruct the sampler to not sample the mipmaps even if they exist
   * (and if the sampling mode is set to a value that normally samples the mipmaps!)
   */
  Property<TextureSampler, bool> useMipMaps;

  /** @hidden */
  std::optional<uint32_t> _cachedWrapU;

  /** @hidden */
  std::optional<uint32_t> _cachedWrapV;

  /** @hidden */
  std::optional<uint32_t> _cachedWrapR;

  /** @hidden */
  std::optional<uint32_t> _cachedAnisotropicFilteringLevel;

  /** @hidden */
  uint32_t _comparisonFunction;

private:
  bool _useMipMaps;

}; // end of class TextureSampler

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_TEXTURE_CONSTANTS_H
