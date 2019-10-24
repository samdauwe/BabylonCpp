#include <babylon/meshes/instanced_lines_mesh.h>

#include <babylon/meshes/lines_mesh.h>

namespace BABYLON {

InstancedLinesMesh::InstancedLinesMesh(const std::string& iName,
                                       const LinesMeshPtr& iSource)
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

InstancedLinesMesh&
InstancedLinesMesh::enableEdgesRendering(float epsilon,
                                         bool checkVerticesInsteadOfIndices)
{
  LinesMesh::enableEdgesRendering(
    std::static_pointer_cast<AbstractMesh>(shared_from_this()), epsilon,
    checkVerticesInsteadOfIndices);
  return *this;
}

} // end of namespace BABYLON
