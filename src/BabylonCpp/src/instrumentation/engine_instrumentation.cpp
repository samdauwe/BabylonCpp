#include <babylon/instrumentation/engine_instrumentation.h>

#include <babylon/engine/engine.h>

namespace BABYLON {

EngineInstrumentation::EngineInstrumentation(Engine* engine)
    : _engine{engine}
    , _captureGPUFrameTime{false}
    , _gpuFrameTimeToken{nullptr}
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

PerfCounter& EngineInstrumentation::gpuFrameTimeCounter()
{
  return _gpuFrameTime;
}

bool EngineInstrumentation::captureGPUFrameTime() const
{
  return _captureGPUFrameTime;
}

void EngineInstrumentation::setCaptureGPUFrameTime(bool value)
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
          _gpuFrameTimeToken = nullptr;
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

PerfCounter& EngineInstrumentation::shaderCompilationTimeCounter()
{
  return _shaderCompilationTime;
}

bool EngineInstrumentation::captureShaderCompilationTime() const
{
  return _captureShaderCompilationTime;
}

void EngineInstrumentation::setCaptureShaderCompilationTime(bool value)
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
