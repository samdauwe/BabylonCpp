#include <babylon/instrumentation/engine_instrumentation.h>

#include <babylon/engines/engine.h>

namespace BABYLON {

EngineInstrumentation::EngineInstrumentation(Engine* engine)
    : gpuFrameTimeCounter{this, &EngineInstrumentation::get_gpuFrameTimeCounter}
    , captureGPUFrameTime{this, &EngineInstrumentation::get_captureGPUFrameTime,
                          &EngineInstrumentation::set_captureGPUFrameTime}
    , shaderCompilationTimeCounter{this, &EngineInstrumentation::get_shaderCompilationTimeCounter}
    , captureShaderCompilationTime{this, &EngineInstrumentation::get_captureShaderCompilationTime,
                                   &EngineInstrumentation::set_captureShaderCompilationTime}
    , _engine{engine}
    , _captureGPUFrameTime{false}
    , _captureShaderCompilationTime{false}
    , _onBeforeShaderCompilationObserver{nullptr}
    , _onAfterShaderCompilationObserver{nullptr}
{
}

EngineInstrumentation::~EngineInstrumentation() = default;

PerfCounter& EngineInstrumentation::get_gpuFrameTimeCounter()
{
  return *_engine->getGPUFrameTimeCounter();
}

bool EngineInstrumentation::get_captureGPUFrameTime() const
{
  return _captureGPUFrameTime;
}

void EngineInstrumentation::set_captureGPUFrameTime(bool value)
{
  if (value == _captureGPUFrameTime) {
    return;
  }

  _captureGPUFrameTime = value;
  _engine->captureGPUFrameTime(value);
}

PerfCounter& EngineInstrumentation::get_shaderCompilationTimeCounter()
{
  return _shaderCompilationTime;
}

bool EngineInstrumentation::get_captureShaderCompilationTime() const
{
  return _captureShaderCompilationTime;
}

void EngineInstrumentation::set_captureShaderCompilationTime(bool value)
{
  if (value == _captureShaderCompilationTime) {
    return;
  }

  _captureShaderCompilationTime = value;

  if (value) {
    _onBeforeShaderCompilationObserver = _engine->onBeforeShaderCompilationObservable.add(
      [this](Engine* /*engine*/, EventState& /*es*/) -> void {
        _shaderCompilationTime.fetchNewFrame();
        _shaderCompilationTime.beginMonitoring();
      });

    _onAfterShaderCompilationObserver = _engine->onAfterShaderCompilationObservable.add(
      [this](Engine* /*engine*/, EventState& /*es*/) -> void {
        _shaderCompilationTime.endMonitoring();
      });
  }
  else {
    _engine->onBeforeShaderCompilationObservable.remove(_onBeforeShaderCompilationObserver);
    _onBeforeShaderCompilationObserver = nullptr;
    _engine->onAfterShaderCompilationObservable.remove(_onAfterShaderCompilationObserver);
    _onAfterShaderCompilationObserver = nullptr;
  }
}

void EngineInstrumentation::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  _engine->onBeforeShaderCompilationObservable.remove(_onBeforeShaderCompilationObserver);
  _onBeforeShaderCompilationObserver = nullptr;

  _engine->onAfterShaderCompilationObservable.remove(_onAfterShaderCompilationObserver);
  _onAfterShaderCompilationObserver = nullptr;

  _engine = nullptr;
}

} // end of namespace BABYLON
