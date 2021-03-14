#ifndef BABYLON_ENGINES_ENGINE_OPTIONS_H
#define BABYLON_ENGINES_ENGINE_OPTIONS_H

#include <optional>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface defining initialization parameters for Engine class.
 */
struct BABYLON_SHARED_EXPORT EngineOptions {
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
   * @see https://doc.babylonjs.com/how_to/webvr_camera
   */
  bool autoEnableWebVR       = false;
  bool preserveDrawingBuffer = true;
  bool stencil               = true;
  /**
   * Defines if webgl2 should be turned off even if supported
   * @see https://doc.babylonjs.com/features/webgl2
   */
  bool disableWebGL2Support = false;
  /**
   * Defines if webaudio should be initialized as well
   * @see https://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  bool audioEngine = false;
  /**
   * Defines if animations should run using a deterministic lock step
   * @see https://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  bool deterministicLockstep = false;
  /**
   * Defines the maximum steps to use with deterministic lock step mode
   */
  unsigned int lockstepMaxSteps = 4;
  /**
   * Defines the seconds between each deterministic lock step
   */
  std::optional<float> timeStep = std::nullopt;
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
  /**
   * Make the canvas XR Compatible for XR sessions
   */
  bool xrCompatible = false;
  /**
   * Make the matrix computations to be performed in 64 bits instead of 32 bits. False by default
   */
  bool useHighPrecisionMatrix = false;

  /**
   * Will prevent the system from falling back to software implementation if a hardware device
   * cannot be created
   */
  bool failIfMajorPerformanceCaveat = false;

  /**
   * Defines whether to adapt to the device's viewport characteristics (default: false)
   */
  bool adaptToDeviceRatio = false;
}; // end of struct EngineOptions

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_OPTIONS_H
