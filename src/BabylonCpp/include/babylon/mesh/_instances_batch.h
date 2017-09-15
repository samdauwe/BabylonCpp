#ifndef BABYLON_MESH_INSTANCES_BATCH_H
#define BABYLON_MESH_INSTANCES_BATCH_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT _InstancesBatch {

public:
  bool mustReturn;
  unordered_map_t<size_t, vector_t<InstancedMesh*>> visibleInstances;
  unordered_map_t<size_t, bool> renderSelf;

}; // end of class InstancesBatch

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_INSTANCES_BATCH_H
