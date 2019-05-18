#include <babylon/tools/optimization/scene_optimizer.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/tools/optimization/scene_optimization.h>

namespace BABYLON {

void SceneOptimizer::_CheckCurrentState(
  Scene* scene, SceneOptimizerOptions& options, int currentPriorityLevel,
  const std::function<void()>& onSuccess,
  const std::function<void()>& onFailure)
{
  if (scene->getEngine()->getFps() >= options.targetFrameRate) {
    if (onSuccess) {
      onSuccess();
    }

    return;
  }

  // Apply current level of optimizations
  bool allDone               = true;
  bool noOptimizationApplied = true;
  for (auto& optimization : options.optimizations) {
    if (optimization.priority == currentPriorityLevel) {
      noOptimizationApplied = false;
      allDone               = allDone && optimization.apply(scene);
    }
  }

  // If no optimization was applied, this is a failure :(
  if (noOptimizationApplied) {
    if (onFailure) {
      onFailure();
    }

    return;
  }

  // If all optimizations were done, move to next level
  if (allDone) {
    ++currentPriorityLevel;
  }

  // Let's the system running for a specific amount of time before checking FPS
  /*scene->executeWhenReady(void() {
      setTimeout(() => {
          SceneOptimizer._CheckCurrentState(scene, options,
                         currentPriorityLevel, onSuccess, onFailure);
      }, options.trackerDuration);
  });*/
}

void SceneOptimizer::OptimizeAsync(Scene* /*scene*/,
                                   const SceneOptimizerOptions& /*options*/,
                                   const std::function<void()>& /*Success*/,
                                   const std::function<void()>& /*onFailure*/)
{
}

} // end of namespace BABYLON
