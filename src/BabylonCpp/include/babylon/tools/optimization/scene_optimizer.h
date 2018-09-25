#ifndef BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZER_H
#define BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZER_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/tools/optimization/scene_optimizer_options.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SceneOptimizer {

  static void
  _CheckCurrentState(Scene* scene, SceneOptimizerOptions& options,
                     int currentPriorityLevel,
                     const std::function<void()>& onSuccess = nullptr,
                     const std::function<void()>& onFailure = nullptr);

  static void
  OptimizeAsync(Scene* scene,
                const SceneOptimizerOptions& options
                = SceneOptimizerOptions::ModerateDegradationAllowed(),
                const std::function<void()>& onSuccess = nullptr,
                const std::function<void()>& onFailure = nullptr);

}; // end of struct SceneOptimizer

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OPTIMIZATION_SCENE_OPTIMIZER_H
