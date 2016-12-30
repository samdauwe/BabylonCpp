#include <babylon/mesh/mesh_lod_level.h>

#include <babylon/mesh/mesh.h>

namespace BABYLON {

MeshLODLevel::MeshLODLevel(float _distance, Mesh* _mesh)
    : distance{_distance}, mesh{_mesh}
{
}

MeshLODLevel::~MeshLODLevel()
{
}

} // end of namespace BABYLON
