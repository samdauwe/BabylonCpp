#ifndef BABYLON_MESH_MESH_LOD_LEVEL_H
#define BABYLON_MESH_MESH_LOD_LEVEL_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT MeshLODLevel {

public:
  MeshLODLevel(float distance, Mesh* mesh);
  ~MeshLODLevel();

public:
  float distance;
  Mesh* mesh;

}; // end of class MeshLODLevel

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_MESH_LOD_LEVEL_H
