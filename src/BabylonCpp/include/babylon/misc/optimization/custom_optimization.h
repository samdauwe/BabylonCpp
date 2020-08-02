#ifndef BABYLON_MISC_OPTIMIZATION_CUSTOM_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_CUSTOM_OPTIMIZATION_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

/**
 * @brief Defines an optimization based on user defined callback.
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT CustomOptimization : public SceneOptimization {

public:
  CustomOptimization(int priority = 0);
  ~CustomOptimization() override; // = default

  /**
   * @brief Gets a string describing the action executed by the current optimization.
   * @return description string
   */
  std::string getDescription() const override;

  /**
   * @brief This function will be called by the SceneOptimizer when its priority is reached in order
   * to apply the change required by the current optimization.
   * @param scene defines the current scene where to apply this optimization
   * @param optimizer defines the current optimizer
   * @returns true if everything that can be done was applied
   */
  bool apply(Scene* scene, SceneOptimizer* optimizer) override;

public:
  /**
   * Callback called to apply the custom optimization.
   */
  std::function<bool(Scene* scene, SceneOptimizer* optimizer)> onApply;

  /**
   * Callback called to get custom description
   */
  std::function<std::string()> onGetDescription;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_CUSTOM_OPTIMIZATION_H
