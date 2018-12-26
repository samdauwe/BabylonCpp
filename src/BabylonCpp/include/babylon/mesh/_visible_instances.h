#ifndef BABYLON_MESH_VISIBLE_INSTANCES_H
#define BABYLON_MESH_VISIBLE_INSTANCES_H

#include <unordered_map>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class InstancedMesh;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _VisibleInstances {
  int defaultRenderId;
  int selfDefaultRenderId;
  int intermediateDefaultRenderId;
  std::unordered_map<int, std::vector<InstancedMesh*>> meshes;
}; // end of struct _VisibleInstances

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_VISIBLE_INSTANCES_H
