#include <babylon/instrumentation/engine_instrumentation.h>

#include <babylon/engine/engine.h>

namespace BABYLON {

EngineInstrumentation::EngineInstrumentation(Engine* engine)
    : gpuFrameTimeCounter{this, &EngineInstrumentation::get_gpuFrameTimeCounter}
    , captureGPUFrameTime{this, &EngineInstrumentation::get_captureGPUFrameTime,
                          &EngineInstrumentation::set_captureGPUFrameTime}
    , shaderCompilationTimeCounter{this, &EngineInstrumentation::
                                           get_shaderCompilationTimeCounter}
    , captureShaderCompilationTime{this,
                                   &EngineInstrumentation::
                                     get_captureShaderCompilationTime,
                                   &EngineInstrumentation::
                                     set_captureShaderCompilationTime}
    , _engine{engine}
    , _captureGPUFrameTime{false}
    , _gpuFrameTimeToken{std::nullopt}
    , _captureShaderCompilationTime{false}
    , _onBeginFrameObserver{nullptr}
    , _onEndFrameObserver{nullptr}
    , _onBeforeShaderCompilationObserver{nullptr}
    , _onAfterShaderCompilationObserver{nullptr}
{
}

EngineInstrumentation::~EngineInstrumentation()
{
}

PerfCounter& EngineInstrumentation::get_gpuFrameTimeCounter()
{
  return _gpuFrameTime;
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

  if (value) {
    _onBeginFrameObserver = _engine->onBeginFrameObservable.add(
      [this](Engine* /*engine*/, EventState& /*es*/) {
        if (!_gpuFrameTimeToken) {
          _gpuFrameTimeToken = _engine->startTimeQuery();
        }
      });

    _onEndFrameObserver = _engine->onEndFrameObservable.add(
      [this](Engine* /*engine*/, EventState& /*es*/) {
        if (!_gpuFrameTimeToken) {
          return;
        }
        auto time = _engine->endTimeQuery(_gpuFrameTimeToken);

        if (time > -1) {
          _gpuFrameTimeToken = std::nullopt;
          _gpuFrameTime.fetchNewFrame();
          _gpuFrameTime.addCount(static_cast<size_t>(time), true);
        }
      });
  }
  else {
    _engine->onBeginFrameObservable.remove(_onBeginFrameObserver);
    _onBeginFrameObserver = nullptr;
    _engine->onEndFrameObservable.remove(_onEndFrameObserver);
    _onEndFrameObserver = nullptr;
  }
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
    _onBeforeShaderCompilationObserver
      = _engine->onBeforeShaderCompilationObservable.add(
        [this](Engine* /*engine*/, EventState& /*es*/) {
          _shaderCompilationTime.fetchNewFrame();
          _shaderCompilationTime.beginMonitoring();
        });

    _onAfterShaderCompilationObserver
      = _engine->onAfterShaderCompilationObservable.add(
        [this](Engine* /*engine*/, EventState& /*es*/) {
          _shaderCompilationTime.endMonitoring();
        });
  }
  else {
    _engine->onBeforeShaderCompilationObservable.remove(
      _onBeforeShaderCompilationObserver);
    _onBeforeShaderCompilationObserver = nullptr;
    _engine->onAfterShaderCompilationObservable.remove(
      _onAfterShaderCompilationObserver);
    _onAfterShaderCompilationObserver = nullptr;
  }
}

void EngineInstrumentation::dispose(bool /*doNotRecurse*/,
                                    bool /*disposeMaterialAndTextures*/)
{
  _engine->onBeginFrameObservable.remove(_onBeginFrameObserver);
  _onBeginFrameObserver = nullptr;

  _engine->onEndFrameObservable.remove(_onEndFrameObserver);
  _onEndFrameObserver = nullptr;

  _engine->onBeforeShaderCompilationObservable.remove(
    _onBeforeShaderCompilationObserver);
  _onBeforeShaderCompilationObserver = nullptr;

  _engine->onAfterShaderCompilationObservable.remove(
    _onAfterShaderCompilationObserver);
  _onAfterShaderCompilationObserver = nullptr;

  _engine = nullptr;
}

} // end of namespace BABYLON
