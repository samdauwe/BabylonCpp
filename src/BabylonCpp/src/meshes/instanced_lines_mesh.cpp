#include <babylon/meshes/instanced_lines_mesh.h>

#include <babylon/meshes/lines_mesh.h>

namespace BABYLON {

InstancedLinesMesh::InstancedLinesMesh(const std::string& iName,
                                       LinesMesh* iSource)
    : InstancedMesh{iName, iSource}
{
}

InstancedLinesMesh::~InstancedLinesMesh()
{
}

const std::string InstancedLinesMesh::getClassName() const
{
  return "InstancedLinesMesh";
}

} // end of namespace BABYLON
