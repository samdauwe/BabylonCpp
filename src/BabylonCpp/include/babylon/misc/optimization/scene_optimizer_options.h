#ifndef BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H
#define BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H

#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SceneOptimizerOptions {

public:
  SceneOptimizerOptions(float targetFrameRate = 60, int trackerDuration = 2000);
  SceneOptimizerOptions(const SceneOptimizerOptions& other);
  SceneOptimizerOptions(SceneOptimizerOptions&& other);
  SceneOptimizerOptions& operator=(const SceneOptimizerOptions& other);
  SceneOptimizerOptions& operator=(SceneOptimizerOptions&& other);
  ~SceneOptimizerOptions();

  static SceneOptimizerOptions LowDegradationAllowed(float targetFrameRate
                                                     = 60);

  static SceneOptimizerOptions ModerateDegradationAllowed(float targetFrameRate
                                                          = 60);

  static SceneOptimizerOptions HighDegradationAllowed(float targetFrameRate
                                                      = 60);

public:
  std::vector<SceneOptimization> optimizations;
  float targetFrameRate;
  int trackerDuration;

}; // end of class SceneOptimizerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H
