#ifndef BABYLON_LAYERS_IHIGHLIGHT_LAYER_OPTIONS_H
#define BABYLON_LAYERS_IHIGHLIGHT_LAYER_OPTIONS_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>

namespace BABYLON {

FWD_CLASS_SPTR(Camera)

/**
 * @brief Highlight layer options. This helps customizing the behaviour
 * of the highlight layer.
 */
struct BABYLON_SHARED_EXPORT IHighlightLayerOptions {

  /**
   * Multiplication factor apply to the canvas size to compute the render target
   * size
   * used to generated the glowing objects (the smaller the faster).
   */
  float mainTextureRatio = 0.5f;

  /**
   * Enforces a fixed size texture to ensure resize independent blur.
   */
  std::optional<int> mainTextureFixedSize = std::nullopt;

  /**
   * Multiplication factor apply to the main texture size in the first step of
   * the blur to reduce the size of the picture to blur (the smaller the faster).
   */
  float blurTextureSizeRatio = 0.5f;

  /**
   * How big in texel of the blur texture is the vertical blur.
   */
  float blurVerticalSize = 1.f;

  /**
   * How big in texel of the blur texture is the horizontal blur.
   */
  float blurHorizontalSize = 1.f;

  /**
   * Alpha blending mode used to apply the blur. Default is combine.
   */
  unsigned int alphaBlendingMode = Constants::ALPHA_COMBINE;

  /**
   * The camera attached to the layer.
   */
  CameraPtr camera = nullptr;

  /**
   * Should we display highlight as a solid stroke?
   */
  std::optional<bool> isStroke = std::nullopt;

  /**
   * The rendering group to draw the layer in.
   */
  int renderingGroupId = -1;

}; // end of struct IHighlightLayerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_LAYERS_IHIGHLIGHT_LAYER_OPTIONS_H
