#ifndef BABYLON_ENGINES_ENGINE_OPTIONS_H
#define BABYLON_ENGINES_ENGINE_OPTIONS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface defining initialization parameters for Engine class.
 */
struct BABYLON_SHARED_EXPORT EngineOptions {
  /**
   * Defines whether to adapt to the device's viewport characteristics (default: false)
   */
  bool adaptToDeviceRatio = false;
  /**
   * Defines enable antialiasing (default: false)
   */
  bool antialias = true;
  /**
   * Defines if the engine should no exceed a specified device ratio
   * @see https://developer.mozilla.org/en-US/docs/Web/API/Window/devicePixelRatio
   */
  int limitDeviceRatio = false;
  /**
   * Defines if webvr should be enabled automatically
   * @see http://doc.babylonjs.com/how_to/webvr_camera
   */
  bool autoEnableWebVR       = false;
  bool preserveDrawingBuffer = true;
  bool stencil               = true;
  /**
   * Defines if webgl2 should be turned off even if supported
   * @see http://doc.babylonjs.com/features/webgl2
   */
  bool disableWebGL2Support = false;
  /**
   * Defines if webaudio should be initialized as well
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  bool audioEngine = false;
  /**
   * Defines if animations should run using a deterministic lock step
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  bool deterministicLockstep = false;
  /**
   * Defines the maximum steps to use with deterministic lock step mode
   */
  unsigned int lockstepMaxSteps = 4;
  /**
   * Defines that engine should ignore modifying touch action attribute and style If not handle, you
   * might need to set it up on your side for expected touch devices behavior.
   */
  bool doNotHandleContextLost = false;
  /**
   * Defines that engine should ignore modifying touch action attribute and style
   * If not handle, you might need to set it up on your side for expected touch devices behavior.
   */
  bool doNotHandleTouchAction = false;
  /**
   * Defines that engine should compile shaders with high precision floats (if supported). True by
   * default
   */
  bool useHighPrecisionFloats = false;
  /**
   * Defines that engine should compile shaders with high precision floats (if supported). True by
   * default
   */
  bool premultipliedAlpha = true;
}; // end of struct EngineOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_OPTIONS_H
