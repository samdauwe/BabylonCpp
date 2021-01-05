#include <babylon/meshes/builders/box_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr BoxBuilder::CreateBox(const std::string& name, BoxOptions& options, Scene* scene)
{
  const auto box = Mesh::New(name, scene);

  options.sideOrientation              = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  box->_originalBuilderSideOrientation = *options.sideOrientation;

  const auto vertexData = VertexData::CreateBox(options);

  vertexData->applyToMesh(*box, options.updatable);

  return box;
}

} // end of namespace BABYLON
