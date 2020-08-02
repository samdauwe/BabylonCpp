#ifndef BABYLON_MISC_OPTIMIZATION_TEXTURE_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_TEXTURE_OPTIMIZATION_H

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

/**
 * @brief Defines an optimization used to reduce the size of render target textures.
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT TextureOptimization : public SceneOptimization {

public:
  /**
   * @brief Creates the TextureOptimization object.
   * @param priority defines the priority of this optimization (0 by default which means first in
   * the list)
   * @param maximumSize defines the maximum sized allowed for textures (1024 is the default value).
   * If a texture is bigger, it will be scaled down using a factor defined by the step parameter
   * @param step defines the factor (0.5 by default) used to scale down textures bigger than maximum
   * sized allowed.
   */
  TextureOptimization(int priority = 0, int maximumSize = 1024, float step = 0.5f);
  ~TextureOptimization() override; // = default

  /**
   * @brief Gets a string describing the action executed by the current optimization.
   * @returns description string
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
   * Defines the priority of this optimization (0 by default which means first in the list)
   */
  int priority;

  /**
   * Defines the maximum sized allowed for textures (1024 is the default value). If a texture is
   * bigger, it will be scaled down using a factor defined by the step parameter
   */
  int maximumSize;

  /**
   * Defines the factor (0.5 by default) used to scale down textures bigger than maximum sized
   * allowed.
   */
  float step;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_TEXTURE_OPTIMIZATION_H
