#ifndef BABYLON_MESHES_MESH_LOD_LEVEL_H
#define BABYLON_MESHES_MESH_LOD_LEVEL_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

FWD_CLASS_SPTR(Mesh)

/**
 * @brief Class used to represent a specific level of detail of a mesh.
 * @see https://doc.babylonjs.com/how_to/how_to_use_lod
 */
class BABYLON_SHARED_EXPORT MeshLODLevel {

public:
  /**
   * @brief Creates a new LOD level.
   * @param distance defines the distance where this level should star being displayed
   * @param mesh defines the mesh to use to render this level
   */
  MeshLODLevel(float distance, const MeshPtr& mesh);
  MeshLODLevel(const MeshLODLevel& other);
  MeshLODLevel(MeshLODLevel&& other);
  MeshLODLevel& operator=(const MeshLODLevel& other);
  MeshLODLevel& operator=(MeshLODLevel&& other);
  ~MeshLODLevel(); // = default

  bool operator==(const MeshLODLevel& other) const;
  bool operator!=(const MeshLODLevel& other) const;

public:
  /**
   * Defines the distance where this level should start being displayed
   */
  float distance;

  /**
   * Defines the mesh to use to render this level
   */
  MeshPtr mesh;

}; // end of class MeshLODLevel

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_MESH_LOD_LEVEL_H
