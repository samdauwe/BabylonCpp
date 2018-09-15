#ifndef BABYLON_INSTRUMENTATION_SCENE_INSTRUMENTATION_H
#define BABYLON_INSTRUMENTATION_SCENE_INSTRUMENTATION_H

#include <babylon/babylon_api.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observer.h>
#include <babylon/tools/perf_counter.h>

namespace BABYLON {

class Camera;
class Scene;

/**
 * @brief This class can be used to get instrumentation data from a Babylon
 * engine.
 */
class BABYLON_SHARED_EXPORT SceneInstrumentation : public IDisposable {

public:
  SceneInstrumentation(Scene* scene);
  virtual ~SceneInstrumentation();

  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  // Properties
  /**
   * @brief Gets the perf counter used for active meshes evaluation time.
   */
  PerfCounter& get_activeMeshesEvaluationTimeCounter();

  /**
   * @brief Gets the active meshes evaluation time capture status.
   */
  bool get_captureActiveMeshesEvaluationTime() const;

  /**
   * @brief Enable or disable the active meshes evaluation time capture.
   */
  void set_captureActiveMeshesEvaluationTime(bool value);

  /**
   * @brief Gets the perf counter used for render targets render time.
   */
  PerfCounter& get_renderTargetsRenderTimeCounter();

  /**
   * @brief Gets the render targets render time capture status.
   */
  bool get_captureRenderTargetsRenderTime() const;

  /**
   * @brief Enable or disable the render targets render time capture.
   */
  void set_captureRenderTargetsRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for particles render time.
   */
  PerfCounter& get_particlesRenderTimeCounter();

  /**
   * @brief Gets the particles render time capture status.
   */
  bool get_captureParticlesRenderTime() const;

  /**
   * @brief Enable or disable the particles render time capture.
   */
  void set_captureParticlesRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for sprites render time.
   */
  PerfCounter& get_spritesRenderTimeCounter();

  /**
   * @brief Gets the sprites render time capture status.
   */
  bool get_captureSpritesRenderTime() const;

  /**
   * @brief Enable or disable the sprites render time capture.
   */
  void set_captureSpritesRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for physics time.
   */
  PerfCounter& get_physicsTimeCounter();

  /**
   * @brief Gets the physics time capture status.
   */
  bool get_capturePhysicsTime() const;

  /**
   * @brief Enable or disable the physics time capture.
   */
  void set_capturePhysicsTime(bool value);

  /**
   * @brief Gets the perf counter used for animations time.
   */
  PerfCounter& get_animationsTimeCounter();

  /**
   * @brief Gets the animations time capture status.
   */
  bool get_captureAnimationsTime() const;

  /**
   * @brief Enable or disable the animations time capture.
   */
  void set_captureAnimationsTime(bool value);

  /**
   * @brief Gets the perf counter used for frame time capture.
   */
  PerfCounter& get_frameTimeCounter();

  /**
   * @brief Gets the frame time capture status.
   */
  bool get_captureFrameTime() const;

  /**
   * @brief Enable or disable the frame time capture.
   */
  void set_captureFrameTime(bool value);

  /**
   * @brief Gets the perf counter used for inter-frames time capture.
   */
  PerfCounter& get_interFrameTimeCounter();

  /**
   * @brief Gets the inter-frames time capture status.
   */
  bool get_captureInterFrameTime() const;

  /**
   * @brief Enable or disable the inter-frames time capture.
   */
  void set_captureInterFrameTime(bool value);

  /**
   * @brief Gets the perf counter used for render time capture.
   */
  PerfCounter& get_renderTimeCounter();

  /**
   * @brief Gets the render time capture status.
   */
  bool get_captureRenderTime() const;

  /**
   * @brief Enable or disable the render time capture.
   */
  void set_captureRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for camera render time capture.
   */
  PerfCounter& get_cameraRenderTimeCounter();

  /**
   * @brief Gets the camera render time capture status.
   */
  bool get_captureCameraRenderTime() const;

  /**
   * @brief Enable or disable the camera render time capture.
   */
  void set_captureCameraRenderTime(bool value);

  /**
   * @brief Gets the perf counter used for draw calls.
   */
  PerfCounter& get_drawCallsCounter();

  /**
   * @brief Gets the perf counter used for texture collisions.
   */
  PerfCounter& get_textureCollisionsCounter();

public:
  // Properties
  /**
   * Perf counter used for active meshes evaluation time.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter>
    activeMeshesEvaluationTimeCounter;

  /**
   * Active meshes evaluation time capture status.
   */
  Property<SceneInstrumentation, bool> captureActiveMeshesEvaluationTime;

  /**
   * Perf counter used for render targets render time.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter>
    renderTargetsRenderTimeCounter;

  /**
   * Render targets render time capture status.
   */
  Property<SceneInstrumentation, bool> captureRenderTargetsRenderTime;

  /**
   * Perf counter used for particles render time.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter>
    particlesRenderTimeCounter;

  /**
   * Particles render time capture status.
   */
  Property<SceneInstrumentation, bool> captureParticlesRenderTime;

  /**
   * Perf counter used for sprites render time.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> spritesRenderTimeCounter;

  /**
   * Sprites render time capture status.
   */
  Property<SceneInstrumentation, bool> captureSpritesRenderTime;

  /**
   * Perf counter used for physics time.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> physicsTimeCounter;

  /**
   * Physics time capture status.
   */
  Property<SceneInstrumentation, bool> capturePhysicsTime;

  /**
   * Perf counter used for animations time.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> animationsTimeCounter;

  /**
   * Animations time capture status.
   */
  Property<SceneInstrumentation, bool> captureAnimationsTime;

  /**
   * Perf counter used for frame time capture.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> frameTimeCounter;

  /**
   * Frame time capture status.
   */
  Property<SceneInstrumentation, bool> captureFrameTime;

  /**
   * Perf counter used for inter-frames time capture.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> interFrameTimeCounter;

  /**
   * Inter-frames time capture status.
   */
  Property<SceneInstrumentation, bool> captureInterFrameTime;

  /**
   * Perf counter used for render time capture.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> renderTimeCounter;

  /**
   * Render time capture status.
   */
  Property<SceneInstrumentation, bool> captureRenderTime;

  /**
   *  Perf counter used for camera render time capture.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> cameraRenderTimeCounter;

  /**
   * Camera render time capture status.
   */
  Property<SceneInstrumentation, bool> captureCameraRenderTime;

  /**
   * Perf counter used for draw calls.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> drawCallsCounter;

  /**
   * Perf counter used for texture collisions.
   */
  ReadOnlyProperty<SceneInstrumentation, PerfCounter> textureCollisionsCounter;

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

  bool _captureCameraRenderTime;
  PerfCounter _cameraRenderTime;

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

  Observer<Camera>::Ptr _onBeforeCameraRenderObserver;
  Observer<Camera>::Ptr _onAfterCameraRenderObserver;

}; // end of class SceneInstrumentation

} // end of namespace BABYLON

#endif // end of BABYLON_INSTRUMENTATION_SCENE_INSTRUMENTATION_H
