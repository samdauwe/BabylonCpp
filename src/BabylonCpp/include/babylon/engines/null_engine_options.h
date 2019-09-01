#ifndef BABYLON_ENGINES_NULL_ENGINE_OPTIONS_H
#define BABYLON_ENGINES_NULL_ENGINE_OPTIONS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Options to create the null engine.
 */
struct BABYLON_SHARED_EXPORT NullEngineOptions {
  /**
   * Render width (Default: 512)
   */
  int renderWidth = 512;
  /**
   * Render height (Default: 256)
   */
  int renderHeight = 256;

  /**
   * Texture size (Default: 512)
   */
  int textureSize = 512;

  /**
   * If delta time between frames should be constant
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  bool deterministicLockstep = false;

  /**
   * Maximum about of steps between frames (Default: 4)
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  unsigned int lockstepMaxSteps = 4;
}; // end of struct NullEngineOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_NULL_ENGINE_OPTIONS_H
