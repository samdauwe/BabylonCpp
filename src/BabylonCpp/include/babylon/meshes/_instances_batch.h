#ifndef BABYLON_MESHES_INSTANCES_BATCH_H
#define BABYLON_MESHES_INSTANCES_BATCH_H

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
  std::vector<bool> hardwareInstancedRendering;
}; // end of struct InstancesBatch

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INSTANCES_BATCH_H
