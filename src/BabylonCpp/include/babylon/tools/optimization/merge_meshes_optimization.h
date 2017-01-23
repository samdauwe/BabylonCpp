#ifndef BABYLON_TOOLS_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H
#define BABYLON_TOOLS_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H

#include <babylon/babylon_global.h>
#include <babylon/tools/optimization/scene_optimization.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MergeMeshesOptimization : public SceneOptimization {

public:
  static bool UpdateSelectionTree();
  static void UpdateSelectionTree(bool value);

public:
  MergeMeshesOptimization(int priority = 0);
  ~MergeMeshesOptimization();

  bool apply(Scene* scene) override;
  bool _apply(Scene* scene, bool updateSelectionTree = false);

private:
  bool _canBeMerged(AbstractMesh* abstractMesh);

private:
  static bool _UpdateSelectionTree;

}; // end of class MergeMeshesOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H
