#ifndef BABYLON_INSTRUMENTATION_ENGINE_INSTRUMENTATION_H
#define BABYLON_INSTRUMENTATION_ENGINE_INSTRUMENTATION_H

#include <babylon/babylon_api.h>
#include <babylon/instrumentation/_time_token.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/misc/observer.h>
#include <babylon/misc/perf_counter.h>

namespace BABYLON {

class Engine;

/**
 * @brief This class can be used to get instrumentation data from a Babylon engine.
 * @see https://doc.babylonjs.com/how_to/optimizing_your_scene#engineinstrumentation
 */
class BABYLON_SHARED_EXPORT EngineInstrumentation : public IDisposable {

public:
  /**
   * @brief Instantiates a new engine instrumentation.
   * This class can be used to get instrumentation data from a Babylon engine
   * @see https://doc.babylonjs.com/how_to/optimizing_your_scene#engineinstrumentation
   * @param engine Defines the engine to instrument
   */
  EngineInstrumentation(Engine* engine);
  ~EngineInstrumentation() override; // = default

  /**
   * @brief Dispose and release associated resources.
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  // Properties
  /**
   * @brief Gets the perf counter used for GPU frame time.
   */
  PerfCounter& get_gpuFrameTimeCounter();

  /**
   * @brief Gets the GPU frame time capture status.
   */
  [[nodiscard]] bool get_captureGPUFrameTime() const;

  /**
   * @brief Enable or disable the GPU frame time capture.
   */
  void set_captureGPUFrameTime(bool value);

  /**
   * @brief Gets the perf counter used for shader compilation time.
   */
  PerfCounter& get_shaderCompilationTimeCounter();

  /**
   * @brief Gets the shader compilation time capture status.
   */
  [[nodiscard]] bool get_captureShaderCompilationTime() const;

  /**
   * @brief Enable or disable the shader compilation time capture.
   */
  void set_captureShaderCompilationTime(bool value);

public:
  // Properties
  /**
   * Perf counter used for GPU frame time.
   */
  ReadOnlyProperty<EngineInstrumentation, PerfCounter> gpuFrameTimeCounter;

  /**
   * Enable or disable the GPU frame time capture.
   */
  Property<EngineInstrumentation, bool> captureGPUFrameTime;

  /**
   * Perf counter used for shader compilation time.
   */
  ReadOnlyProperty<EngineInstrumentation, PerfCounter> shaderCompilationTimeCounter;

  /**
   * Enable or disable the shader compilation time capture.
   */
  Property<EngineInstrumentation, bool> captureShaderCompilationTime;

private:
  /**
   * Define the instrumented engine.
   */
  Engine* _engine;

  bool _captureGPUFrameTime = false;
  std::optional<_TimeToken> _gpuFrameTimeToken;
  PerfCounter _gpuFrameTime;

  bool _captureShaderCompilationTime;
  PerfCounter _shaderCompilationTime;

  // Observers
  Observer<Engine>::Ptr _onBeginFrameObserver;
  Observer<Engine>::Ptr _onEndFrameObserver;
  Observer<Engine>::Ptr _onBeforeShaderCompilationObserver;
  Observer<Engine>::Ptr _onAfterShaderCompilationObserver;

}; // end of class EngineInstrumentation

} // end of namespace BABYLON

#endif // end of BABYLON_INSTRUMENTATION_ENGINE_INSTRUMENTATION_H
