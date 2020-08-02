#ifndef BABYLON_MISC_OPTIMIZATION_HARDWARE_SCALING_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_HARDWARE_SCALING_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

/**
 * @brief Defines an optimization used to increase or decrease the rendering resolution.
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT HardwareScalingOptimization : public SceneOptimization {

public:
  /**
   * @brief Creates the HardwareScalingOptimization object.
   * @param priority defines the priority of this optimization (0 by default which means first in
   * the list)
   * @param maximumScale defines the maximum scale to use (2 by default)
   * @param step defines the step to use between two passes (0.5 by default)
   */
  HardwareScalingOptimization(int priority = 0, float maximumScale = 2.f, float step = 0.25f);
  ~HardwareScalingOptimization() override; // = default

  /**
   * @brief Gets a string describing the action executed by the current optimization
   * @return description string
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

public:
  /**
   * Defines the priority of this optimization (0 by default which means first in the list)
   */
  int priority;

  /**
   * Defines the maximum scale to use (2 by default)
   */
  float maximumScale;

  /**
   * Defines the step to use between two passes (0.5 by default)
   */
  float step;

private:
  float _currentScale;
  float _directionOffset;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_HARDWARE_SCALING_OPTIMIZATION_H
