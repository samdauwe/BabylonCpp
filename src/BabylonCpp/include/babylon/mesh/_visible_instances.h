#ifndef BABYLON_MESH_VISIBLE_INSTANCES_H
#define BABYLON_MESH_VISIBLE_INSTANCES_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT _VisibleInstances {

public:
  int defaultRenderId;
  int selfDefaultRenderId;
  int intermediateDefaultRenderId;
  std::unordered_map<int, std::vector<InstancedMesh*>> meshes;

}; // end of class _VisibleInstances

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_VISIBLE_INSTANCES_H
