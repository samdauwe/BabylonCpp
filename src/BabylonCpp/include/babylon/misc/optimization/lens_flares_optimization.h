#ifndef BABYLON_MISC_OPTIMIZATION_LENS_FLARES_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_LENS_FLARES_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

/**
 * @brief Defines an optimization used to turn lens flares off.
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT LensFlaresOptimization : public SceneOptimization {

public:
  LensFlaresOptimization(int priority = 0);
  ~LensFlaresOptimization() override; // = default

  /**
   * @brief Gets a string describing the action executed by the current optimization.
   * @returns description string
   */
  std::string getDescription() const override;

  /**
   * @brief This function will be called by the SceneOptimizer when its priority is reached in order
   * to apply the change required by the current optimization
   * @param scene defines the current scene where to apply this optimization
   * @param optimizer defines the current optimizer
   * @returns true if everything that can be done was applied
   */
  bool apply(Scene* scene, SceneOptimizer* optimizer) override;

}; // end of class LensFlaresOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_LENS_FLARES_OPTIMIZATION_H
