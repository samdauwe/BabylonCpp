#ifndef BABYLON_MISC_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H
#define BABYLON_MISC_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/misc/optimization/scene_optimization.h>

namespace BABYLON {

class AbstractMesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

class BABYLON_SHARED_EXPORT MergeMeshesOptimization : public SceneOptimization {

public:
  static bool UpdateSelectionTree();
  static void UpdateSelectionTree(bool value);

public:
  MergeMeshesOptimization(int priority = 0);
  ~MergeMeshesOptimization() override;

  bool apply(Scene* scene) override;
  bool _apply(Scene* scene, bool updateSelectionTree = false);

private:
  bool _canBeMerged(const AbstractMeshPtr& abstractMesh);

private:
  static bool _UpdateSelectionTree;

}; // end of class MergeMeshesOptimization

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_OPTIMIZATION_MERGE_MESHES_OPTIMIZATION_H
