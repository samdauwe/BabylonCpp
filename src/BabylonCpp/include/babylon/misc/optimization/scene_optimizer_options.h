#ifndef BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H
#define BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H

#include <functional>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

/**
 * @brief Defines a list of options used by SceneOptimizer.
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT SceneOptimizerOptions {

public:
  /**
   * @brief Creates a new list of options used by SceneOptimizer.
   * @param targetFrameRate defines the target frame rate to reach (60 by default)
   * @param trackerDuration defines the interval between two checkes (2000ms by default)
   */
  SceneOptimizerOptions(float targetFrameRate = 60.f, int trackerDuration = 2000);
  SceneOptimizerOptions(const SceneOptimizerOptions& other);
  SceneOptimizerOptions(SceneOptimizerOptions&& other);
  SceneOptimizerOptions& operator=(const SceneOptimizerOptions& other);
  SceneOptimizerOptions& operator=(SceneOptimizerOptions&& other);
  ~SceneOptimizerOptions(); // = default

  /**
   * @brief Add a new optimization.
   * @param optimization defines the SceneOptimization to add to the list of active optimizations
   * @returns the current SceneOptimizerOptions
   */
  SceneOptimizerOptions& addOptimization(const SceneOptimization& optimization);

  /**
   * @brief Add a new custom optimization.
   * @param onApply defines the callback called to apply the custom optimization (true if everything
   * that can be done was applied)
   * @param onGetDescription defines the callback called to get the description attached with the
   * optimization.
   * @param priority defines the priority of this optimization (0 by default which means first in
   * the list)
   * @returns the current SceneOptimizerOptions
   */
  SceneOptimizerOptions&
  addCustomOptimization(const std::function<bool(Scene* scene, SceneOptimizer* optimizer)>& onApply,
                        const std::function<std::string()>& onGetDescription, int priority);

  /**
   * @brief Creates a list of pre-defined optimizations aimed to reduce the visual impact on the
   * scene.
   * @param targetFrameRate defines the target frame rate (60 by default)
   * @returns a SceneOptimizerOptions object
   */
  static SceneOptimizerOptions LowDegradationAllowed(float targetFrameRate = 60.f);

  /**
   * @brief Creates a list of pre-defined optimizations aimed to have a moderate impact on the scene
   * visual.
   * @param targetFrameRate defines the target frame rate (60 by default)
   * @returns a SceneOptimizerOptions object
   */
  static SceneOptimizerOptions ModerateDegradationAllowed(float targetFrameRate = 60.f);

  /**
   * @brief Creates a list of pre-defined optimizations aimed to have a big impact on the scene
   * visual.
   * @param targetFrameRate defines the target frame rate (60 by default)
   * @returns a SceneOptimizerOptions object
   */
  static SceneOptimizerOptions HighDegradationAllowed(float targetFrameRate = 60.f);

public:
  /**
   * Gets the list of optimizations to apply
   */
  std::vector<SceneOptimization> optimizations;

  /**
   * Defines the target frame rate to reach (60 by default)
   */
  float targetFrameRate;

  /**
   * Defines the interval between two checkes (2000ms by default)
   */
  int trackerDuration;

}; // end of class SceneOptimizerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZER_OPTIONS_H
