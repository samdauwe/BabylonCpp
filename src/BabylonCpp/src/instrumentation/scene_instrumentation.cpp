#include <babylon/instrumentation/scene_instrumentation.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

SceneInstrumentation::SceneInstrumentation(Scene* iScene)
    : scene{iScene}
    , activeMeshesEvaluationTimeCounter{this,
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

      scene->getEngine()->_drawCalls.fetchNewFrame();
      scene->getEngine()->_textureCollisions.fetchNewFrame();
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
      = scene->onBeforeActiveMeshesEvaluationObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Active meshes evaluation");
          _activeMeshesEvaluationTime.beginMonitoring();
        });

    _onAfterActiveMeshesEvaluationObserver
      = scene->onAfterActiveMeshesEvaluationObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Active meshes evaluation");
          _activeMeshesEvaluationTime.endMonitoring();
        });
  }
  else {
    scene->onBeforeActiveMeshesEvaluationObservable.remove(
      _onBeforeActiveMeshesEvaluationObserver);
    _onBeforeActiveMeshesEvaluationObserver = nullptr;

    scene->onAfterActiveMeshesEvaluationObservable.remove(
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
      = scene->onBeforeRenderTargetsRenderObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Render targets rendering");
          _renderTargetsRenderTime.beginMonitoring();
        });

    _onAfterRenderTargetsRenderObserver
      = scene->onAfterRenderTargetsRenderObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Render targets rendering");
          _renderTargetsRenderTime.endMonitoring(false);
        });
  }
  else {
    scene->onBeforeRenderTargetsRenderObservable.remove(
      _onBeforeRenderTargetsRenderObserver);
    _onBeforeRenderTargetsRenderObserver = nullptr;

    scene->onAfterRenderTargetsRenderObservable.remove(
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
      = scene->onBeforeParticlesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Particles");
          _particlesRenderTime.beginMonitoring();
        });

    _onAfterParticlesRenderingObserver
      = scene->onAfterParticlesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Particles");
          _particlesRenderTime.endMonitoring(false);
        });
  }
  else {
    scene->onBeforeParticlesRenderingObservable.remove(
      _onBeforeParticlesRenderingObserver);
    _onBeforeParticlesRenderingObserver = nullptr;

    scene->onAfterParticlesRenderingObservable.remove(
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

  if (scene->spriteManagers.empty()) {
    return;
  }

  if (value) {
    _onBeforeSpritesRenderingObserver
      = scene->onBeforeSpritesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::StartPerformanceCounter("Sprites");
          _spritesRenderTime.beginMonitoring();
        });

    _onAfterSpritesRenderingObserver
      = scene->onAfterSpritesRenderingObservable.add(
        [this](Scene* /*scene*/, EventState& /*es*/) {
          Tools::EndPerformanceCounter("Sprites");
          _spritesRenderTime.endMonitoring(false);
        });
  }
  else {
    scene->onBeforeSpritesRenderingObservable.remove(
      _onBeforeSpritesRenderingObserver);
    _onBeforeSpritesRenderingObserver = nullptr;

    scene->onAfterSpritesRenderingObservable.remove(
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
    _onBeforePhysicsObserver = scene->onBeforePhysicsObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        Tools::StartPerformanceCounter("Physics");
        _physicsTime.beginMonitoring();
      });

    _onAfterPhysicsObserver = scene->onAfterPhysicsObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        Tools::EndPerformanceCounter("Physics");
        _physicsTime.endMonitoring();
      });
  }
  else {
    scene->onBeforePhysicsObservable.remove(_onBeforePhysicsObserver);
    _onBeforePhysicsObserver = nullptr;

    scene->onAfterPhysicsObservable.remove(_onAfterPhysicsObserver);
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
    _onAfterAnimationsObserver = scene->onAfterAnimationsObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        _animationsTime.endMonitoring();
      });
  }
  else {
    scene->onAfterAnimationsObservable.remove(_onAfterAnimationsObserver);
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
    _onBeforeDrawPhaseObserver = scene->onBeforeDrawPhaseObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        _renderTime.beginMonitoring();
        Tools::StartPerformanceCounter("Main render");
      });

    _onAfterDrawPhaseObserver = scene->onAfterDrawPhaseObservable.add(
      [this](Scene* /*scene*/, EventState& /*es*/) {
        _renderTime.endMonitoring(false);
        Tools::EndPerformanceCounter("Main render");
      });
  }
  else {
    scene->onBeforeDrawPhaseObservable.remove(_onBeforeDrawPhaseObserver);
    _onBeforeDrawPhaseObserver = nullptr;
    scene->onAfterDrawPhaseObservable.remove(_onAfterDrawPhaseObserver);
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
    _onBeforeCameraRenderObserver = scene->onBeforeCameraRenderObservable.add(
      [this](Camera* camera, EventState& /*es*/) {
        _cameraRenderTime.beginMonitoring();
        Tools::StartPerformanceCounter("Rendering camera " + camera->name);
      });

    _onAfterCameraRenderObserver = scene->onAfterCameraRenderObservable.add(
      [this](Camera* camera, EventState& /*es*/) {
        _cameraRenderTime.endMonitoring(false);
        Tools::EndPerformanceCounter("Rendering camera " + camera->name);
      });
  }
  else {
    scene->onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
    _onBeforeCameraRenderObserver = nullptr;
    scene->onAfterCameraRenderObservable.remove(_onAfterCameraRenderObserver);
    _onAfterCameraRenderObserver = nullptr;
  }
}

PerfCounter& SceneInstrumentation::get_drawCallsCounter()
{
  return scene->getEngine()->_drawCalls;
}

PerfCounter& SceneInstrumentation::get_textureCollisionsCounter()
{
  return scene->getEngine()->_textureCollisions;
}

void SceneInstrumentation::dispose(bool /*doNotRecurse*/,
                                   bool /*disposeMaterialAndTextures*/)
{
  scene->onAfterRenderObservable.remove(_onAfterRenderObserver);
  _onAfterRenderObserver = nullptr;

  scene->onBeforeActiveMeshesEvaluationObservable.remove(
    _onBeforeActiveMeshesEvaluationObserver);
  _onBeforeActiveMeshesEvaluationObserver = nullptr;

  scene->onAfterActiveMeshesEvaluationObservable.remove(
    _onAfterActiveMeshesEvaluationObserver);
  _onAfterActiveMeshesEvaluationObserver = nullptr;

  scene->onBeforeRenderTargetsRenderObservable.remove(
    _onBeforeRenderTargetsRenderObserver);
  _onBeforeRenderTargetsRenderObserver = nullptr;

  scene->onAfterRenderTargetsRenderObservable.remove(
    _onAfterRenderTargetsRenderObserver);
  _onAfterRenderTargetsRenderObserver = nullptr;

  scene->onBeforeAnimationsObservable.remove(_onBeforeAnimationsObserver);
  _onBeforeAnimationsObserver = nullptr;

  scene->onBeforeParticlesRenderingObservable.remove(
    _onBeforeParticlesRenderingObserver);
  _onBeforeParticlesRenderingObserver = nullptr;

  scene->onAfterParticlesRenderingObservable.remove(
    _onAfterParticlesRenderingObserver);
  _onAfterParticlesRenderingObserver = nullptr;

  scene->onBeforeSpritesRenderingObservable.remove(
    _onBeforeSpritesRenderingObserver);
  _onBeforeSpritesRenderingObserver = nullptr;

  scene->onAfterSpritesRenderingObservable.remove(
    _onAfterSpritesRenderingObserver);
  _onAfterSpritesRenderingObserver = nullptr;

  scene->onBeforeDrawPhaseObservable.remove(_onBeforeDrawPhaseObserver);
  _onBeforeDrawPhaseObserver = nullptr;

  scene->onAfterDrawPhaseObservable.remove(_onAfterDrawPhaseObserver);
  _onAfterDrawPhaseObserver = nullptr;

  scene->onBeforePhysicsObservable.remove(_onBeforePhysicsObserver);
  _onBeforePhysicsObserver = nullptr;

  scene->onAfterPhysicsObservable.remove(_onAfterPhysicsObserver);
  _onAfterPhysicsObserver = nullptr;

  scene->onAfterAnimationsObservable.remove(_onAfterAnimationsObserver);
  _onAfterAnimationsObserver = nullptr;

  scene->onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
  _onBeforeCameraRenderObserver = nullptr;

  scene->onAfterCameraRenderObservable.remove(_onAfterCameraRenderObserver);
  _onAfterCameraRenderObserver = nullptr;

  scene = nullptr;
}

} // end of namespace BABYLON
