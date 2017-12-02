#ifndef BABYLON_INSTRUMENTATION_SCENE_INSTRUMENTATION_H
#define BABYLON_INSTRUMENTATION_SCENE_INSTRUMENTATION_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observer.h>
#include <babylon/tools/perf_counter.h>

namespace BABYLON {

/**
 * @brief This class can be used to get instrumentation data from a Babylon
 * engine.
 */
class BABYLON_SHARED_EXPORT SceneInstrumentation : public IDisposable {

public:
  SceneInstrumentation(Scene* scene);
  virtual ~SceneInstrumentation();

  // Properties

  /**
   * @brief Gets the perf counter used for active meshes evaluation time.
   */
  PerfCounter& activeMeshesEvaluationTimeCounter();

  /**
   * @brief Gets the active meshes evaluation time capture status.
   */
  bool captureActiveMeshesEvaluationTime() const;

  /**
   * @brief Enable or disable the active meshes evaluation time capture.
   */
  void setCaptureActiveMeshesEvaluationTime(bool value);

  /**
   * @brief Gets the perf counter used for render targets render time.
   */
  PerfCounter& renderTargetsRenderTimeCounter();

  /**
   * @brief Gets the render targets render time capture status.
   */
  bool captureRenderTargetsRenderTime() const;

  /**
   * @brief Enable or disable the render targets render time capture.
   */
  void setCaptureRenderTargetsRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for particles render time.
   */
  PerfCounter& particlesRenderTimeCounter();

  /**
   * @brief Gets the particles render time capture status.
   */
  bool captureParticlesRenderTime() const;

  /**
   * @brief Enable or disable the particles render time capture.
   */
  void setCaptureParticlesRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for sprites render time.
   */
  PerfCounter& spritesRenderTimeCounter();

  /**
   * @brief Gets the sprites render time capture status.
   */
  bool captureSpritesRenderTime() const;

  /**
   * @brief Enable or disable the sprites render time capture.
   */
  void setCaptureSpritesRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for physics time.
   */
  PerfCounter& physicsTimeCounter();

  /**
   * @brief Gets the physics time capture status.
   */
  bool capturePhysicsTime() const;

  /**
   * @brief Enable or disable the physics time capture.
   */
  void setCapturePhysicsTime(bool value);

  /**
   * @brief Gets the perf counter used for animations time.
   */
  PerfCounter& animationsTimeCounter();

  /**
   * @brief Gets the animations time capture status.
   */
  bool captureAnimationsTime() const;

  /**
   * @brief Enable or disable the animations time capture.
   */
  void setCaptureAnimationsTime(bool value);

  /**
   * @brief Gets the perf counter used for frame time capture.
   */
  PerfCounter& frameTimeCounter();

  /**
   * @brief Gets the frame time capture status.
   */
  bool captureFrameTime() const;

  /**
   * @brief Enable or disable the frame time capture.
   */
  void setCaptureFrameTime(bool value);

  /**
   * @brief Gets the perf counter used for inter-frames time capture.
   */
  PerfCounter& interFrameTimeCounter();

  /**
   * @brief Gets the inter-frames time capture status.
   */
  bool captureInterFrameTime() const;

  /**
   * @brief Enable or disable the inter-frames time capture.
   */
  void setCaptureInterFrameTime(bool value);

  /**
   * @brief Gets the perf counter used for render time capture.
   */
  PerfCounter& renderTimeCounter();

  /**
   * @brief Gets the render time capture status.
   */
  bool captureRenderTime() const;

  /**
   * @brief Enable or disable the render time capture.
   */
  void setCaptureRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for frame time capture.
   */
  PerfCounter& drawCallsCounter();

  void dispose(bool doNotRecurse = false) override;

private:
  Scene* _scene;

  bool _captureActiveMeshesEvaluationTime;
  PerfCounter _activeMeshesEvaluationTime;

  bool _captureRenderTargetsRenderTime;
  PerfCounter _renderTargetsRenderTime;

  bool _captureFrameTime;
  PerfCounter _frameTime;

  bool _captureRenderTime;
  PerfCounter _renderTime;

  bool _captureInterFrameTime;
  PerfCounter _interFrameTime;

  bool _captureParticlesRenderTime;
  PerfCounter _particlesRenderTime;

  bool _captureSpritesRenderTime;
  PerfCounter _spritesRenderTime;

  bool _capturePhysicsTime;
  PerfCounter _physicsTime;

  bool _captureAnimationsTime;
  PerfCounter _animationsTime;

  // Observers
  Observer<Scene>::Ptr _onBeforeActiveMeshesEvaluationObserver;
  Observer<Scene>::Ptr _onAfterActiveMeshesEvaluationObserver;
  Observer<Scene>::Ptr _onBeforeRenderTargetsRenderObserver;
  Observer<Scene>::Ptr _onAfterRenderTargetsRenderObserver;

  Observer<Scene>::Ptr _onAfterRenderObserver;

  Observer<Scene>::Ptr _onBeforeDrawPhaseObserver;
  Observer<Scene>::Ptr _onAfterDrawPhaseObserver;

  Observer<Scene>::Ptr _onBeforeAnimationsObserver;

  Observer<Scene>::Ptr _onBeforeParticlesRenderingObserver;
  Observer<Scene>::Ptr _onAfterParticlesRenderingObserver;

  Observer<Scene>::Ptr _onBeforeSpritesRenderingObserver;
  Observer<Scene>::Ptr _onAfterSpritesRenderingObserver;

  Observer<Scene>::Ptr _onBeforePhysicsObserver;
  Observer<Scene>::Ptr _onAfterPhysicsObserver;

  Observer<Scene>::Ptr _onAfterAnimationsObserver;

}; // end of class SceneInstrumentation

} // end of namespace BABYLON

#endif // end of BABYLON_INSTRUMENTATION_SCENE_INSTRUMENTATION_H
