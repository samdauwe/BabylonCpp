#include <babylon/meshes/builders/box_builder.h>

#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/meshes/vertex_data_options.h>

namespace BABYLON {

MeshPtr BoxBuilder::CreateBox(const std::string& name, BoxOptions& options,
                              Scene* scene)
{
  auto box = Mesh::New(name, scene);

  options.sideOrientation
    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  box->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateBox(options);

  vertexData->applyToMesh(*box, options.updatable);

  return box;
}

} // end of namespace BABYLON
