#ifndef BABYLON_LAYER_IHIGHLIGHT_LAYER_OPTIONS_H
#define BABYLON_LAYER_IHIGHLIGHT_LAYER_OPTIONS_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine.h>

namespace BABYLON {

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
  float mainTextureRatio = 0.25f;

  /**
   * Enforces a fixed size texture to ensure resize independant blur.
   */
  int mainTextureFixedSize;

  /**
   * Multiplication factor apply to the main texture size in the first step of
   * the blur to reduce the size
   * of the picture to blur (the smaller the faster).
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
  int alphaBlendingMode = Engine::ALPHA_COMBINE;

  /**
   * The camera attached to the layer.
   */
  Camera* camera = nullptr;

}; // end of struct IHighlightLayerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_IHIGHLIGHT_LAYER_OPTIONS_H
