#ifndef BABYLON_LAYERS_IGLOW_LAYER_OPTIONS_H
#define BABYLON_LAYERS_IGLOW_LAYER_OPTIONS_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Camera;
using CameraPtr = std::shared_ptr<Camera>;

/**
 * @brief Glow layer options. This helps customizing the behaviour of the glow
 * layer.
 */
struct BABYLON_SHARED_EXPORT IGlowLayerOptions {

  /**
   * Multiplication factor apply to the canvas size to compute the render target
   * size used to generated the glowing objects (the smaller the faster).
   */
  float mainTextureRatio;

  /**
   * Enforces a fixed size texture to ensure resize independant blur.
   */
  std::optional<int> mainTextureFixedSize = std::nullopt;

  /**
   * How big is the kernel of the blur texture.
   */
  int blurKernelSize = 32;

  /**
   * The camera attached to the layer.
   */
  CameraPtr camera = nullptr;

  /**
   * Enable MSAA by chosing the number of samples.
   */
  std::optional<int> mainTextureSamples = 1;

  /**
   * The rendering group to draw the layer in.
   */
  int renderingGroupId = -1;

}; // end of struct IGlowLayerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_LAYERS_IGLOW_LAYER_OPTIONS_H
