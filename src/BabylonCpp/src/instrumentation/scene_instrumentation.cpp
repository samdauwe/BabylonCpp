#include <babylon/instrumentation/scene_instrumentation.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

SceneInstrumentation::SceneInstrumentation(Scene* scene)
    : _scene{scene}
    , _captureActiveMeshesEvaluationTime{false}
    , _captureRenderTargetsRenderTime{false}
    , _captureFrameTime{false}
    , _captureRenderTime{false}
    , _captureInterFrameTime{false}
    , _captureParticlesRenderTime{false}
    , _captureSpritesRenderTime{false}
    , _capturePhysicsTime{false}
    , _captureAnimationsTime{false}
    , _onBeforeActiveMeshesEvaluationObserver{nullptr}
    , _onAfterActiveMeshesEvaluationObserver{nullptr}
    , _onBeforeRenderTargetsRenderObserver{nullptr}
    , _onAfterRenderTargetsRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _onBeforeDrawPhaseObserver{nullptr}
    , _onAfterDrawPhaseObserver{nullptr}
    , _onBeforeAnimationsObserver{nullptr}
    , _onBeforeParticlesRenderingObserver{nullptr}
    , _onAfterParticlesRenderingObserver{nullptr}
    , _onBeforeSpritesRenderingObserver{nullptr}
    , _onAfterSpritesRenderingObserver{nullptr}
    , _onBeforePhysicsObserver{nullptr}
    , _onAfterPhysicsObserver{nullptr}
    , _onAfterAnimationsObserver{nullptr}
{
  // Before render
  _onBeforeAnimationsObserver = scene->onBeforeAnimationsObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) {
      if (_captureActiveMeshesEvaluationTime) {
        _activeMeshesEvaluationTime.fetchNewFrame();
      }

      if (_captureRenderTargetsRenderTime) {
        _renderTargetsRenderTime.fetchNewFrame();
      }

      if (_captureFrameTime) {
        Tools::StartPerformanceCounter("Scene rendering");
        _frameTime.beginMonitoring();
      }

      if (_captureInterFrameTime) {
        _interFrameTime.endMonitoring();
      }

      if (_captureParticlesRenderTime) {
        _particlesRenderTime.fetchNewFrame();
      }

      if (_captureSpritesRenderTime) {
        _spritesRenderTime.fetchNewFrame();
      }

      if (_captureAnimationsTime) {
        _animationsTime.beginMonitoring();
      }

      _scene->getEngine()->_drawCalls.fetchNewFrame();
      _scene->getEngine()->_textureCollisions.fetchNewFrame();
    });

  // After render
  _onAfterRenderObserver = scene->onAfterRenderObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) {
      if (_captureFrameTime) {
        Tools::EndPerformanceCounter("Scene rendering");
        _frameTime.endMonitoring();
      }

      if (_captureRenderTime) {
        _renderTime.endMonitoring(false);
      }

      if (_captureInterFrameTime) {
        _interFrameTime.beginMonitoring();
      }
    });
}

SceneInstrumentation::~SceneInstrumentation()
{
}

PerfCounter& SceneInstrumentation::activeMeshesEvaluationTimeCounter()
{
  return _activeMeshesEvaluationTime;
}

bool SceneInstrumentation::captureActiveMeshesEvaluationTime() const
{
  return _captureActiveMeshesEvaluationTime;
}

void SceneInstrumentation::setCaptureActiveMeshesEvaluationTime(bool value)
{
  if (value == _captureActiveMeshesEvaluationTime) {
    return;
  }

  _captureActiveMeshesEvaluationTime = value;

  if (value) {
    _onBeforeActiveMeshesEvaluationObserver
      = _scene->onBeforeActiveMeshesEvaluationObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Active meshes evaluation");
          _activeMeshesEvaluationTime.beginMonitoring();
        });

    _onAfterActiveMeshesEvaluationObserver
      = _scene->onAfterActiveMeshesEvaluationObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Active meshes evaluation");
          _activeMeshesEvaluationTime.endMonitoring();
        });
  }
  else {
    _scene->onBeforeActiveMeshesEvaluationObservable.remove(
      _onBeforeActiveMeshesEvaluationObserver);
    _onBeforeActiveMeshesEvaluationObserver = nullptr;

    _scene->onAfterActiveMeshesEvaluationObservable.remove(
      _onAfterActiveMeshesEvaluationObserver);
    _onAfterActiveMeshesEvaluationObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::renderTargetsRenderTimeCounter()
{
  return _renderTargetsRenderTime;
}

bool SceneInstrumentation::captureRenderTargetsRenderTime() const
{
  return _captureRenderTargetsRenderTime;
}

void SceneInstrumentation::setCaptureRenderTargetsRenderTime(bool value)
{
  if (value == _captureRenderTargetsRenderTime) {
    return;
  }

  _captureRenderTargetsRenderTime = value;

  if (value) {
    _onBeforeRenderTargetsRenderObserver
      = _scene->OnBeforeRenderTargetsRenderObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Render targets rendering");
          _renderTargetsRenderTime.beginMonitoring();
        });

    _onAfterRenderTargetsRenderObserver
      = _scene->OnAfterRenderTargetsRenderObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Render targets rendering");
          _renderTargetsRenderTime.endMonitoring(false);
        });
  }
  else {
    _scene->OnBeforeRenderTargetsRenderObservable.remove(
      _onBeforeRenderTargetsRenderObserver);
    _onBeforeRenderTargetsRenderObserver = nullptr;

    _scene->OnAfterRenderTargetsRenderObservable.remove(
      _onAfterRenderTargetsRenderObserver);
    _onAfterRenderTargetsRenderObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::particlesRenderTimeCounter()
{
  return _particlesRenderTime;
}

bool SceneInstrumentation::captureParticlesRenderTime() const
{
  return _captureParticlesRenderTime;
}

void SceneInstrumentation::setCaptureParticlesRenderTime(bool value)
{
  if (value == _captureParticlesRenderTime) {
    return;
  }

  _captureParticlesRenderTime = value;

  if (value) {
    _onBeforeParticlesRenderingObserver
      = _scene->onBeforeParticlesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Particles");
          _particlesRenderTime.beginMonitoring();
        });

    _onAfterParticlesRenderingObserver
      = _scene->onAfterParticlesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Particles");
          _particlesRenderTime.endMonitoring(false);
        });
  }
  else {
    _scene->onBeforeParticlesRenderingObservable.remove(
      _onBeforeParticlesRenderingObserver);
    _onBeforeParticlesRenderingObserver = nullptr;

    _scene->onAfterParticlesRenderingObservable.remove(
      _onAfterParticlesRenderingObserver);
    _onAfterParticlesRenderingObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::spritesRenderTimeCounter()
{
  return _spritesRenderTime;
}

bool SceneInstrumentation::captureSpritesRenderTime() const
{
  return _captureSpritesRenderTime;
}

void SceneInstrumentation::setCaptureSpritesRenderTime(bool value)
{
  if (value == _captureSpritesRenderTime) {
    return;
  }

  _captureSpritesRenderTime = value;

  if (value) {
    _onBeforeSpritesRenderingObserver
      = _scene->onBeforeSpritesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Sprites");
          _spritesRenderTime.beginMonitoring();
        });

    _onAfterSpritesRenderingObserver
      = _scene->onAfterSpritesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Sprites");
          _spritesRenderTime.endMonitoring(false);
        });
  }
  else {
    _scene->onBeforeSpritesRenderingObservable.remove(
      _onBeforeSpritesRenderingObserver);
    _onBeforeSpritesRenderingObserver = nullptr;

    _scene->onAfterSpritesRenderingObservable.remove(
      _onAfterSpritesRenderingObserver);
    _onAfterSpritesRenderingObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::physicsTimeCounter()
{
  return _physicsTime;
}

bool SceneInstrumentation::capturePhysicsTime() const
{
  return _capturePhysicsTime;
}

void SceneInstrumentation::setCapturePhysicsTime(bool value)
{
  if (value == _capturePhysicsTime) {
    return;
  }

  _capturePhysicsTime = value;

  if (value) {
    _onBeforePhysicsObserver = _scene->onBeforePhysicsObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        Tools::StartPerformanceCounter("Physics");
        _physicsTime.beginMonitoring();
      });

    _onAfterPhysicsObserver = _scene->onAfterPhysicsObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        Tools::EndPerformanceCounter("Physics");
        _physicsTime.endMonitoring();
      });
  }
  else {
    _scene->onBeforePhysicsObservable.remove(_onBeforePhysicsObserver);
    _onBeforePhysicsObserver = nullptr;

    _scene->onAfterPhysicsObservable.remove(_onAfterPhysicsObserver);
    _onAfterPhysicsObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::animationsTimeCounter()
{
  return _animationsTime;
}

bool SceneInstrumentation::captureAnimationsTime() const
{
  return _captureAnimationsTime;
}

void SceneInstrumentation::setCaptureAnimationsTime(bool value)
{
  if (value == _captureAnimationsTime) {
    return;
  }

  _captureAnimationsTime = value;

  if (value) {
    _onAfterAnimationsObserver = _scene->onAfterAnimationsObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        _animationsTime.endMonitoring();
      });
  }
  else {
    _scene->onAfterAnimationsObservable.remove(_onAfterAnimationsObserver);
    _onAfterAnimationsObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::frameTimeCounter()
{
  return _frameTime;
}

bool SceneInstrumentation::captureFrameTime() const
{
  return _captureFrameTime;
}

void SceneInstrumentation::setCaptureFrameTime(bool value)
{
  _captureFrameTime = value;
}

PerfCounter& SceneInstrumentation::interFrameTimeCounter()
{
  return _interFrameTime;
}

bool SceneInstrumentation::captureInterFrameTime() const
{
  return _captureInterFrameTime;
}

void SceneInstrumentation::setCaptureInterFrameTime(bool value)
{
  _captureInterFrameTime = value;
}

PerfCounter& SceneInstrumentation::renderTimeCounter()
{
  return _renderTime;
}

bool SceneInstrumentation::captureRenderTime() const
{
  return _captureRenderTime;
}

void SceneInstrumentation::setCaptureRenderTime(bool value)
{
  if (value == _captureRenderTime) {
    return;
  }

  _captureRenderTime = value;

  if (value) {
    _onBeforeDrawPhaseObserver = _scene->onBeforeDrawPhaseObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        _renderTime.beginMonitoring();
        Tools::StartPerformanceCounter("Main render");
      });

    _onAfterDrawPhaseObserver = _scene->onAfterDrawPhaseObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        _renderTime.endMonitoring(false);
        Tools::EndPerformanceCounter("Main render");
      });
  }
  else {
    _scene->onBeforeDrawPhaseObservable.remove(_onBeforeDrawPhaseObserver);
    _onBeforeDrawPhaseObserver = nullptr;
    _scene->onAfterDrawPhaseObservable.remove(_onAfterDrawPhaseObserver);
    _onAfterDrawPhaseObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::drawCallsCounter()
{
  return _scene->getEngine()->_drawCalls;
}

PerfCounter& SceneInstrumentation::textureCollisionsCounter()
{
  return _scene->getEngine()->_textureCollisions;
}

void SceneInstrumentation::dispose(bool /*doNotRecurse*/)
{
  _scene->onAfterRenderObservable.remove(_onAfterRenderObserver);
  _onAfterRenderObserver = nullptr;

  _scene->onBeforeActiveMeshesEvaluationObservable.remove(
    _onBeforeActiveMeshesEvaluationObserver);
  _onBeforeActiveMeshesEvaluationObserver = nullptr;

  _scene->onAfterActiveMeshesEvaluationObservable.remove(
    _onAfterActiveMeshesEvaluationObserver);
  _onAfterActiveMeshesEvaluationObserver = nullptr;

  _scene->OnBeforeRenderTargetsRenderObservable.remove(
    _onBeforeRenderTargetsRenderObserver);
  _onBeforeRenderTargetsRenderObserver = nullptr;

  _scene->OnAfterRenderTargetsRenderObservable.remove(
    _onAfterRenderTargetsRenderObserver);
  _onAfterRenderTargetsRenderObserver = nullptr;

  _scene->onBeforeAnimationsObservable.remove(_onBeforeAnimationsObserver);
  _onBeforeAnimationsObserver = nullptr;

  _scene->onBeforeParticlesRenderingObservable.remove(
    _onBeforeParticlesRenderingObserver);
  _onBeforeParticlesRenderingObserver = nullptr;

  _scene->onAfterParticlesRenderingObservable.remove(
    _onAfterParticlesRenderingObserver);
  _onAfterParticlesRenderingObserver = nullptr;

  _scene->onBeforeSpritesRenderingObservable.remove(
    _onBeforeSpritesRenderingObserver);
  _onBeforeSpritesRenderingObserver = nullptr;

  _scene->onAfterSpritesRenderingObservable.remove(
    _onAfterSpritesRenderingObserver);
  _onAfterSpritesRenderingObserver = nullptr;

  _scene->onBeforeDrawPhaseObservable.remove(_onBeforeDrawPhaseObserver);
  _onBeforeDrawPhaseObserver = nullptr;

  _scene->onAfterDrawPhaseObservable.remove(_onAfterDrawPhaseObserver);
  _onAfterDrawPhaseObserver = nullptr;

  _scene->onBeforePhysicsObservable.remove(_onBeforePhysicsObserver);
  _onBeforePhysicsObserver = nullptr;

  _scene->onAfterPhysicsObservable.remove(_onAfterPhysicsObserver);
  _onAfterPhysicsObserver = nullptr;

  _scene->onAfterAnimationsObservable.remove(_onAfterAnimationsObserver);
  _onAfterAnimationsObserver = nullptr;

  _scene = nullptr;
}

} // end of namespace BABYLON
