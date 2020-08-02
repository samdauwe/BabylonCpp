#ifndef BABYLON_MISC_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

/**
 * @brief Defines an optimization used to merge meshes with compatible materials.
 * @description More details at https://doc.babylonjs.com/how_to/how_to_use_sceneoptimizer
 */
class BABYLON_SHARED_EXPORT MergeMeshesOptimization : public SceneOptimization {

public:
  /**
   * Gets or sets a boolean which defines if optimization octree has to be updated
   */
  static bool UpdateSelectionTree();

  /**
   * Gets or sets a boolean which defines if optimization octree has to be updated
   */
  static void setUpdateSelectionTree(bool value);

public:
  MergeMeshesOptimization(int priority = 0);
  ~MergeMeshesOptimization() override; // = default

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

private:
  bool _canBeMerged(const AbstractMeshPtr& abstractMesh);
  bool _apply(Scene* scene, bool updateSelectionTree = false);

private:
  static bool _UpdateSelectionTree;

}; // end of class MergeMeshesOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H
