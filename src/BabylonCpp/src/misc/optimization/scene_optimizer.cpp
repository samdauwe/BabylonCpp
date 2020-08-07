#include <babylon/misc/optimization/scene_optimizer.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/engine_store.h>
#include <babylon/engines/scene.h>
#include <babylon/misc/optimization/scene_optimization.h>
#include <babylon/misc/optimization/scene_optimizer.h>

namespace BABYLON {

SceneOptimizer::SceneOptimizer(Scene* scene, const std::optional<SceneOptimizerOptions>& options,
                               bool autoGeneratePriorities, bool improvementMode)
    : isInImprovementMode{this, &SceneOptimizer::get_isInImprovementMode}
    , currentPriorityLevel{this, &SceneOptimizer::get_currentPriorityLevel}
    , currentFrameRate{this, &SceneOptimizer::get_currentFrameRate}
    , targetFrameRate{this, &SceneOptimizer::get_targetFrameRate,
                      &SceneOptimizer::set_targetFrameRate}
    , trackerDuration{this, &SceneOptimizer::get_trackerDuration,
                      &SceneOptimizer::set_trackerDuration}
    , optimizations{this, &SceneOptimizer::get_optimizations}
    , _isRunning{false}
    , _currentPriorityLevel{0}
    , _targetFrameRate{60.f}
    , _trackerDuration{2000}
    , _currentFrameRate{0.f}
    , _sceneDisposeObserver{nullptr}
    , _improvementMode{false}
{
  _options = options.value_or(SceneOptimizerOptions{});

  if (_options.targetFrameRate) {
    _targetFrameRate = _options.targetFrameRate;
  }

  if (_options.trackerDuration) {
    _trackerDuration = _options.trackerDuration;
  }

  if (autoGeneratePriorities) {
    auto priority = 0;
    for (auto& optim : _options.optimizations) {
      optim.priority = priority++;
    }
  }

  _improvementMode = improvementMode;
  _scene           = scene ? scene : EngineStore::LastCreatedScene();
  _sceneDisposeObserver
    = _scene->onDisposeObservable.add([this](Scene* /*scene*/, EventState & /*es*/) -> void {
        _sceneDisposeObserver = nullptr;
        dispose();
      });
}

SceneOptimizer::~SceneOptimizer() = default;

bool SceneOptimizer::get_isInImprovementMode() const
{
  return _improvementMode;
}

int SceneOptimizer::get_currentPriorityLevel() const
{
  return _currentPriorityLevel;
}

float SceneOptimizer::get_currentFrameRate() const
{
  return _currentFrameRate;
}

float SceneOptimizer::get_targetFrameRate() const
{
  return _targetFrameRate;
}

void SceneOptimizer::set_targetFrameRate(float value)
{
  _targetFrameRate = value;
}

int SceneOptimizer::get_trackerDuration() const
{
  return _trackerDuration;
}

void SceneOptimizer::set_trackerDuration(int value)
{
  _trackerDuration = value;
}

std::vector<SceneOptimization>& SceneOptimizer::get_optimizations()
{
  return _options.optimizations;
}

void SceneOptimizer::stop()
{
  _isRunning = false;
}

void SceneOptimizer::reset()
{
  _currentPriorityLevel = 0;
}

void SceneOptimizer::start()
{
  if (_isRunning) {
    return;
  }

  _isRunning = true;

  // Let's wait for the scene to be ready before running our check
  _scene->executeWhenReady([this](Scene* /*scene*/, EventState & /*es*/) -> void {
    // setTimeout(() => {
    _checkCurrentState();
    // }, _trackerDuration);
  });
}

void SceneOptimizer::_checkCurrentState()
{
  if (!_isRunning) {
    return;
  }

  auto scene    = _scene;
  auto& options = _options;

  _currentFrameRate = std::round(scene->getEngine()->getFps());

  if ((_improvementMode && _currentFrameRate <= _targetFrameRate)
      || (!_improvementMode && _currentFrameRate >= _targetFrameRate)) {
    _isRunning = false;
    onSuccessObservable.notifyObservers(this);
    return;
  }

  // Apply current level of optimizations
  auto allDone               = true;
  auto noOptimizationApplied = true;
  for (auto& optimization : options.optimizations) {
    if (optimization.priority == _currentPriorityLevel) {
      noOptimizationApplied = false;
      allDone               = allDone && optimization.apply(scene, this);
      onNewOptimizationAppliedObservable.notifyObservers(&optimization);
    }
  }

  // If no optimization was applied, this is a failure :(
  if (noOptimizationApplied) {
    _isRunning = false;
    onFailureObservable.notifyObservers(this);

    return;
  }

  // If all optimizations were done, move to next level
  if (allDone) {
    _currentPriorityLevel++;
  }

  // Let's the system running for a specific amount of time before checking FPS
  scene->executeWhenReady([this](Scene* /*scene*/, EventState & /*es*/) -> void {
    // setTimeout(() => {
    _checkCurrentState();
    // }, _trackerDuration);
  });
}

void SceneOptimizer::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  stop();
  onSuccessObservable.clear();
  onFailureObservable.clear();
  onNewOptimizationAppliedObservable.clear();
  if (_sceneDisposeObserver) {
    _scene->onDisposeObservable.remove(_sceneDisposeObserver);
  }
}

SceneOptimizerPtr SceneOptimizer::OptimizeAsync(Scene* scene,
                                                const std::optional<SceneOptimizerOptions>& options,
                                                const std::function<void()>& onSuccess,
                                                const std::function<void()>& onFailure)
{
  auto optimizer = std::make_shared<SceneOptimizer>(scene, options, false);

  if (onSuccess) {
    optimizer->onSuccessObservable.add([onSuccess](SceneOptimizer* /*sceneOptimizer*/,
                                                   EventState & /*es*/) -> void { onSuccess(); });
  }

  if (onFailure) {
    optimizer->onFailureObservable.add([onFailure](SceneOptimizer* /*sceneOptimizer*/,
                                                   EventState & /*es*/) -> void { onFailure(); });
  }

  optimizer->start();

  return optimizer;
}

} // end of namespace BABYLON
