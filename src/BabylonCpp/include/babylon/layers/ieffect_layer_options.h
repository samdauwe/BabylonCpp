#ifndef BABYLON_LAYERS_IEFFECT_LAYER_OPTIONS_H
#define BABYLON_LAYERS_IEFFECT_LAYER_OPTIONS_H

#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>

namespace BABYLON {

class Camera;
using CameraPtr = std::shared_ptr<Camera>;

/**
 * @brief Effect layer options. This helps customizing the behaviour of the
 * effect layer.
 */
struct BABYLON_SHARED_EXPORT IEffectLayerOptions {

  /**
   * Multiplication factor apply to the canvas size to compute the render target
   * size used to generated the objects (the smaller the faster).
   */
  float mainTextureRatio = 0.5f;

  /**
   * Enforces a fixed size texture to ensure effect stability across devices.
   */
  std::optional<int> mainTextureFixedSize = std::nullopt;

  /**
   * Alpha blending mode used to apply the blur. Default depends of the
   * implementation.
   */
  unsigned int alphaBlendingMode = Constants::ALPHA_COMBINE;

  /**
   * The camera attached to the layer.
   */
  CameraPtr camera = nullptr;

  /**
   * The rendering group to draw the layer in.
   */
  int renderingGroupId = -1;

}; // end of struct IEffectLayerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_LAYERS_IEFFECT_LAYER_OPTIONS_H
