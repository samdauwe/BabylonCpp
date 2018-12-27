#ifndef BABYLON_MESH_INSTANCES_BATCH_H
#define BABYLON_MESH_INSTANCES_BATCH_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class InstancedMesh;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _InstancesBatch {
  bool mustReturn = false;
  std::unordered_map<size_t, std::vector<InstancedMesh*>> visibleInstances;
  std::unordered_map<size_t, bool> renderSelf;
}; // end of struct InstancesBatch

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_INSTANCES_BATCH_H
