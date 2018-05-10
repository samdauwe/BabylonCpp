#include <babylon/tools/optimization/merge_meshes_optimization.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

bool MergeMeshesOptimization::_UpdateSelectionTree = false;

bool MergeMeshesOptimization::UpdateSelectionTree()
{
  return MergeMeshesOptimization::_UpdateSelectionTree;
}

void MergeMeshesOptimization::UpdateSelectionTree(bool value)
{
  MergeMeshesOptimization::_UpdateSelectionTree = value;
}

MergeMeshesOptimization::MergeMeshesOptimization(int iPriority)
    : SceneOptimization{iPriority}
{
}

MergeMeshesOptimization::~MergeMeshesOptimization()
{
}

bool MergeMeshesOptimization::_canBeMerged(AbstractMesh* abstractMesh)
{
  // Check if instance of Mesh
  if (!((abstractMesh->type() == IReflect::Type::MESH)
        || (abstractMesh->type() == IReflect::Type::GROUNDMESH)
        || (abstractMesh->type() == IReflect::Type::LINESMESH))) {
    return false;
  }

  auto mesh = dynamic_cast<Mesh*>(abstractMesh);

  if (!mesh->isVisible || !mesh->isEnabled()) {
    return false;
  }

  if (mesh->instances.size() > 0) {
    return false;
  }

  if (mesh->skeleton() || mesh->hasLODLevels()) {
    return false;
  }

  if (mesh->parent()) {
    return false;
  }

  return true;
}

bool MergeMeshesOptimization::apply(Scene* scene)
{
  return _apply(scene, false);
}

bool MergeMeshesOptimization::_apply(Scene* scene, bool updateSelectionTree)
{

  auto globalPool   = scene->getMeshes();
  auto globalLength = globalPool.size();

  for (size_t index = 0; index < globalLength; ++index) {
    vector_t<Mesh*> currentPool;
    auto current = globalPool[index];

    // Checks
    if (!_canBeMerged(dynamic_cast<Mesh*>(current))) {
      continue;
    }

    currentPool.emplace_back(dynamic_cast<Mesh*>(current));

    // Find compatible meshes
    for (size_t subIndex = index + 1; subIndex < globalLength; ++subIndex) {
      auto otherMesh = globalPool[subIndex];

      if (!_canBeMerged(otherMesh)) {
        continue;
      }

      if (otherMesh->material() != current->material()) {
        continue;
      }

      if (otherMesh->checkCollisions() != current->checkCollisions()) {
        continue;
      }

      currentPool.emplace_back(static_cast<Mesh*>(otherMesh));
      --globalLength;

      stl_util::splice(globalPool, static_cast<int>(subIndex), 1);

      --subIndex;
    }

    if (currentPool.size() < 2) {
      continue;
    }

    // Merge meshes
    Mesh::MergeMeshes(currentPool, true, true);
  }

  if (updateSelectionTree) {
    if (updateSelectionTree) {
      scene->createOrUpdateSelectionOctree();
    }
  }
  else if (MergeMeshesOptimization::UpdateSelectionTree()) {
    scene->createOrUpdateSelectionOctree();
  }

  return true;
}

} // end of namespace BABYLON
