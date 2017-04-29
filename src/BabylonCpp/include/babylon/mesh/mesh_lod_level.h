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
  MeshLODLevel(const MeshLODLevel& other);
  MeshLODLevel(MeshLODLevel&& other);
  MeshLODLevel& operator=(const MeshLODLevel& other);
  MeshLODLevel& operator=(MeshLODLevel&& other);
  ~MeshLODLevel();

  bool operator==(const MeshLODLevel& other) const;
  bool operator!=(const MeshLODLevel& other) const;

public:
  float distance;
  Mesh* mesh;

}; // end of class MeshLODLevel

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_MESH_LOD_LEVEL_H
