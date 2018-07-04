#include <babylon/instrumentation/scene_instrumentation.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

SceneInstrumentation::SceneInstrumentation(Scene* scene)
    : activeMeshesEvaluationTimeCounter{this,
                                        &SceneInstrumentation::
                                          get_activeMeshesEvaluationTimeCounter}
    , captureActiveMeshesEvaluationTime{this,
                                        &SceneInstrumentation::
                                          get_captureActiveMeshesEvaluationTime,
                                        &SceneInstrumentation::
                                          set_captureActiveMeshesEvaluationTime}
    , renderTargetsRenderTimeCounter{this, &SceneInstrumentation::
                                             get_renderTargetsRenderTimeCounter}
    , captureRenderTargetsRenderTime{this,
                                     &SceneInstrumentation::
                                       get_captureRenderTargetsRenderTime,
                                     &SceneInstrumentation::
                                       set_captureRenderTargetsRenderTime}
    , particlesRenderTimeCounter{this, &SceneInstrumentation::
                                         get_particlesRenderTimeCounter}
    , captureParticlesRenderTime{this,
                                 &SceneInstrumentation::
                                   get_captureParticlesRenderTime,
                                 &SceneInstrumentation::
                                   set_captureParticlesRenderTime}
    , spritesRenderTimeCounter{this, &SceneInstrumentation::
                                       get_spritesRenderTimeCounter}
    , captureSpritesRenderTime{this,
                               &SceneInstrumentation::
                                 get_captureSpritesRenderTime,
                               &SceneInstrumentation::
                                 set_captureSpritesRenderTime}
    , physicsTimeCounter{this, &SceneInstrumentation::get_physicsTimeCounter}
    , capturePhysicsTime{this, &SceneInstrumentation::get_capturePhysicsTime,
                         &SceneInstrumentation::set_capturePhysicsTime}
    , animationsTimeCounter{this,
                            &SceneInstrumentation::get_animationsTimeCounter}
    , captureAnimationsTime{this,
                            &SceneInstrumentation::get_captureAnimationsTime,
                            &SceneInstrumentation::set_captureAnimationsTime}
    , frameTimeCounter{this, &SceneInstrumentation::get_frameTimeCounter}
    , captureFrameTime{this, &SceneInstrumentation::get_captureFrameTime,
                       &SceneInstrumentation::set_captureFrameTime}
    , interFrameTimeCounter{this,
                            &SceneInstrumentation::get_interFrameTimeCounter}
    , captureInterFrameTime{this,
                            &SceneInstrumentation::get_captureInterFrameTime,
                            &SceneInstrumentation::set_captureInterFrameTime}
    , renderTimeCounter{this, &SceneInstrumentation::get_renderTimeCounter}
    , captureRenderTime{this, &SceneInstrumentation::get_captureRenderTime,
                        &SceneInstrumentation::set_captureRenderTime}
    , cameraRenderTimeCounter{this, &SceneInstrumentation::
                                      get_cameraRenderTimeCounter}
    , captureCameraRenderTime{this,
                              &SceneInstrumentation::
                                get_captureCameraRenderTime,
                              &SceneInstrumentation::
                                set_captureCameraRenderTime}
    , drawCallsCounter{this, &SceneInstrumentation::get_drawCallsCounter}
    , textureCollisionsCounter{this, &SceneInstrumentation::
                                       get_textureCollisionsCounter}
    , _scene{scene}
    , _captureActiveMeshesEvaluationTime{false}
    , _captureRenderTargetsRenderTime{false}
    , _captureFrameTime{false}
    , _captureRenderTime{false}
    , _captureInterFrameTime{false}
    , _captureParticlesRenderTime{false}
    , _captureSpritesRenderTime{false}
    , _capturePhysicsTime{false}
    , _captureAnimationsTime{false}
    , _captureCameraRenderTime{false}
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
    , _onBeforeCameraRenderObserver{nullptr}
    , _onAfterCameraRenderObserver{nullptr}
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

PerfCounter& SceneInstrumentation::get_activeMeshesEvaluationTimeCounter()
{
  return _activeMeshesEvaluationTime;
}

bool SceneInstrumentation::get_captureActiveMeshesEvaluationTime() const
{
  return _captureActiveMeshesEvaluationTime;
}

void SceneInstrumentation::set_captureActiveMeshesEvaluationTime(bool value)
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

PerfCounter& SceneInstrumentation::get_renderTargetsRenderTimeCounter()
{
  return _renderTargetsRenderTime;
}

bool SceneInstrumentation::get_captureRenderTargetsRenderTime() const
{
  return _captureRenderTargetsRenderTime;
}

void SceneInstrumentation::set_captureRenderTargetsRenderTime(bool value)
{
  if (value == _captureRenderTargetsRenderTime) {
    return;
  }

  _captureRenderTargetsRenderTime = value;

  if (value) {
    _onBeforeRenderTargetsRenderObserver
      = _scene->onBeforeRenderTargetsRenderObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Render targets rendering");
          _renderTargetsRenderTime.beginMonitoring();
        });

    _onAfterRenderTargetsRenderObserver
      = _scene->onAfterRenderTargetsRenderObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Render targets rendering");
          _renderTargetsRenderTime.endMonitoring(false);
        });
  }
  else {
    _scene->onBeforeRenderTargetsRenderObservable.remove(
      _onBeforeRenderTargetsRenderObserver);
    _onBeforeRenderTargetsRenderObserver = nullptr;

    _scene->onAfterRenderTargetsRenderObservable.remove(
      _onAfterRenderTargetsRenderObserver);
    _onAfterRenderTargetsRenderObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::get_particlesRenderTimeCounter()
{
  return _particlesRenderTime;
}

bool SceneInstrumentation::get_captureParticlesRenderTime() const
{
  return _captureParticlesRenderTime;
}

void SceneInstrumentation::set_captureParticlesRenderTime(bool value)
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

PerfCounter& SceneInstrumentation::get_spritesRenderTimeCounter()
{
  return _spritesRenderTime;
}

bool SceneInstrumentation::get_captureSpritesRenderTime() const
{
  return _captureSpritesRenderTime;
}

void SceneInstrumentation::set_captureSpritesRenderTime(bool value)
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

PerfCounter& SceneInstrumentation::get_physicsTimeCounter()
{
  return _physicsTime;
}

bool SceneInstrumentation::get_capturePhysicsTime() const
{
  return _capturePhysicsTime;
}

void SceneInstrumentation::set_capturePhysicsTime(bool value)
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

PerfCounter& SceneInstrumentation::get_animationsTimeCounter()
{
  return _animationsTime;
}

bool SceneInstrumentation::get_captureAnimationsTime() const
{
  return _captureAnimationsTime;
}

void SceneInstrumentation::set_captureAnimationsTime(bool value)
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

PerfCounter& SceneInstrumentation::get_frameTimeCounter()
{
  return _frameTime;
}

bool SceneInstrumentation::get_captureFrameTime() const
{
  return _captureFrameTime;
}

void SceneInstrumentation::set_captureFrameTime(bool value)
{
  _captureFrameTime = value;
}

PerfCounter& SceneInstrumentation::get_interFrameTimeCounter()
{
  return _interFrameTime;
}

bool SceneInstrumentation::get_captureInterFrameTime() const
{
  return _captureInterFrameTime;
}

void SceneInstrumentation::set_captureInterFrameTime(bool value)
{
  _captureInterFrameTime = value;
}

PerfCounter& SceneInstrumentation::get_renderTimeCounter()
{
  return _renderTime;
}

bool SceneInstrumentation::get_captureRenderTime() const
{
  return _captureRenderTime;
}

void SceneInstrumentation::set_captureRenderTime(bool value)
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

PerfCounter& SceneInstrumentation::get_cameraRenderTimeCounter()
{
  return _cameraRenderTime;
}

bool SceneInstrumentation::get_captureCameraRenderTime() const
{
  return _captureCameraRenderTime;
}

void SceneInstrumentation::set_captureCameraRenderTime(bool value)
{
  if (value == _captureCameraRenderTime) {
    return;
  }

  _captureCameraRenderTime = value;

  if (value) {
    _onBeforeCameraRenderObserver = _scene->onBeforeCameraRenderObservable.add(
      [this](Camera* camera, EventState& /*es*/) {
        _cameraRenderTime.beginMonitoring();
        Tools::StartPerformanceCounter("Rendering camera " + camera->name);
      });

    _onAfterCameraRenderObserver = _scene->onAfterCameraRenderObservable.add(
      [this](Camera* camera, EventState& /*es*/) {
        _cameraRenderTime.endMonitoring(false);
        Tools::EndPerformanceCounter("Rendering camera " + camera->name);
      });
  }
  else {
    _scene->onBeforeCameraRenderObservable.remove(
      _onBeforeCameraRenderObserver);
    _onBeforeCameraRenderObserver = nullptr;
    _scene->onAfterCameraRenderObservable.remove(_onAfterCameraRenderObserver);
    _onAfterCameraRenderObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::get_drawCallsCounter()
{
  return _scene->getEngine()->_drawCalls;
}

PerfCounter& SceneInstrumentation::get_textureCollisionsCounter()
{
  return _scene->getEngine()->_textureCollisions;
}

void SceneInstrumentation::dispose(bool /*doNotRecurse*/,
                                   bool /*disposeMaterialAndTextures*/)
{
  _scene->onAfterRenderObservable.remove(_onAfterRenderObserver);
  _onAfterRenderObserver = nullptr;

  _scene->onBeforeActiveMeshesEvaluationObservable.remove(
    _onBeforeActiveMeshesEvaluationObserver);
  _onBeforeActiveMeshesEvaluationObserver = nullptr;

  _scene->onAfterActiveMeshesEvaluationObservable.remove(
    _onAfterActiveMeshesEvaluationObserver);
  _onAfterActiveMeshesEvaluationObserver = nullptr;

  _scene->onBeforeRenderTargetsRenderObservable.remove(
    _onBeforeRenderTargetsRenderObserver);
  _onBeforeRenderTargetsRenderObserver = nullptr;

  _scene->onAfterRenderTargetsRenderObservable.remove(
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

  _scene->onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
  _onBeforeCameraRenderObserver = nullptr;

  _scene->onAfterCameraRenderObservable.remove(_onAfterCameraRenderObserver);
  _onAfterCameraRenderObserver = nullptr;

  _scene = nullptr;
}

} // end of namespace BABYLON
