#ifndef BABYLON_INSTRUMENTATION_ENGINE_INSTRUMENTATION_H
#define BABYLON_INSTRUMENTATION_ENGINE_INSTRUMENTATION_H

#include <babylon/babylon_global.h>
#include <babylon/instrumentation/_time_token.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observer.h>
#include <babylon/tools/perf_counter.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

/**
 * @brief This class can be used to get instrumentation data from a Babylon
 * engine.
 */
class BABYLON_SHARED_EXPORT EngineInstrumentation : public IDisposable {

public:
  EngineInstrumentation(Engine* engine);
  virtual ~EngineInstrumentation();

  // Properties

  /**
   * @brief Gets the perf counter used for GPU frame time.
   */
  PerfCounter& gpuFrameTimeCounter();

  /**
   * @brief Gets the GPU frame time capture status.
   */
  bool captureGPUFrameTime() const;

  /**
   * @brief Enable or disable the GPU frame time capture.
   */
  void setCaptureGPUFrameTime(bool value);

  /**
   * @brief Gets the perf counter used for shader compilation time.
   */
  PerfCounter& shaderCompilationTimeCounter();

  /**
   * @brief Gets the shader compilation time capture status.
   */
  bool captureShaderCompilationTime() const;

  /**
   * @brief Enable or disable the shader compilation time capture.
   */
  void setCaptureShaderCompilationTime(bool value);

  void dispose(bool doNotRecurse = false) override;

private:
  Engine* _engine;

  bool _captureGPUFrameTime = false;
  Nullable<_TimeToken> _gpuFrameTimeToken;
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
