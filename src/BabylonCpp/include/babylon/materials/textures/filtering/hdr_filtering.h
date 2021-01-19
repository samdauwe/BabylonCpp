#ifndef BABYLON_MATERIALS_TEXTURES_FILTERING_HDR_FILTERING_H
#define BABYLON_MATERIALS_TEXTURES_FILTERING_HDR_FILTERING_H

#include <functional>
#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class Effect;
class ThinEngine;
FWD_CLASS_SPTR(BaseTexture)
FWD_CLASS_SPTR(EffectRenderer)
FWD_STRUCT_SPTR(EffectWrapper)
FWD_CLASS_SPTR(InternalTexture)

/**
 * Options for texture filtering
 */
struct BABYLON_SHARED_EXPORT IHDRFilteringOptions {
  /**
   * Scales pixel intensity for the input HDR map.
   */
  std::optional<float> hdrScale = std::nullopt;

  /**
   * Quality of the filter. Should be `Constants.TEXTURE_FILTERING_QUALITY_OFFLINE` for prefiltering
   */
  std::optional<unsigned int> quality = std::nullopt;
}; // end of struct IHDRFilteringOptions

/**
 * @brief Filters HDR maps to get correct renderings of PBR reflections.
 */
class BABYLON_SHARED_EXPORT HDRFiltering {

public:
  /**
   * @brief Instantiates HDR filter for reflection maps.
   *
   * @param engine Thin engine
   * @param options Options
   */
  HDRFiltering(ThinEngine* engine, const IHDRFilteringOptions& options = {});
  ~HDRFiltering() = default;

  /**
   * @brief Get a value indicating if the filter is ready to be used.
   * @param texture Texture to filter
   * @returns true if the filter is ready
   */
  bool isReady(const BaseTexturePtr& texture) const;

  /**
   * @brief Prefilters a cube texture to have mipmap levels representing roughness values.
   * Prefiltering will be invoked at the end of next rendering pass.
   * This has to be done once the map is loaded, and has not been prefiltered by a third party
   * software. See
   * http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
   * for more information
   * @param texture Texture to filter
   * @param onFinished Callback when filtering is done
   * @return Promise called when prefiltering is done
   */
  void prefilter(const BaseTexturePtr& texture, const std::function<void()>& onFinished = nullptr);

private:
  InternalTexturePtr _createRenderTarget(int size);
  BaseTexturePtr _prefilterInternal(const BaseTexturePtr& texture);
  EffectWrapperPtr _createEffect(const BaseTexturePtr& texture,
                                 const std::function<void(Effect* effect)>& onCompiled = nullptr);

public:
  /**
   * Quality switch for prefiltering. Should be set to `Constants.TEXTURE_FILTERING_QUALITY_OFFLINE`
   * unless you care about baking speed.
   */
  unsigned int quality;

  /**
   * Scales pixel intensity for the input HDR map.
   */
  float hdrScale;

private:
  ThinEngine* _engine;
  EffectRendererPtr _effectRenderer;
  EffectWrapperPtr _effectWrapper;

  float _lodGenerationOffset;
  float _lodGenerationScale;

}; // end of class HDRFiltering

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_FILTERING_HDR_FILTERING_H
