#ifndef BABYLON_MATERIALS_IEFFECT_RENDERER_OPTIONS_H
#define BABYLON_MATERIALS_IEFFECT_RENDERER_OPTIONS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Effect Render Options.
 */
struct BABYLON_SHARED_EXPORT IEffectRendererOptions {
  /**
   * Defines the vertices positions.
   */
  Float32Array positions;
  /**
   * Defines the indices.
   */
  IndicesArray indices;
}; // end of struct IEffectRendererOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_IEFFECT_RENDERER_OPTIONS_H
