#ifndef BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZATION_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Scene;
class SceneOptimizer;

/**
 * @brief Defines the root class used to create scene optimization to use with SceneOptimizer
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT SceneOptimization {

public:
  /**
   * @brief Creates the SceneOptimization object.
   * @param priority defines the priority of this optimization (0 by default which means first in
   * the list)
   * @param desc defines the description associated with the optimization
   */
  SceneOptimization(int priority = 0);
  virtual ~SceneOptimization(); // = default

  /**
   * @brief Gets a string describing the action executed by the current optimization.
   * @returns description string
   */
  virtual std::string getDescription() const;

  /**
   * @brief This function will be called by the SceneOptimizer when its priority is reached in order
   * to apply the change required by the current optimization.
   * @param scene defines the current scene where to apply this optimization
   * @param optimizer defines the current optimizer
   * @returns true if everything that can be done was applied
   */
  virtual bool apply(Scene* scene, SceneOptimizer* optimizer);

public:
  /**
   * Defines the priority of this optimization (0 by default which means first in the list)
   */
  int priority;

}; // end of class SceneOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_SCENE_OPTIMIZATION_H
