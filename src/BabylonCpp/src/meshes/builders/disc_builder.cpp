#include <babylon/meshes/builders/disc_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr DiscBuilder::CreateDisc(const std::string& name, DiscOptions& options, Scene* scene)
{
  auto disc = Mesh::New(name, scene);

  options.sideOrientation               = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  disc->_originalBuilderSideOrientation = *options.sideOrientation;

  auto vertexData = VertexData::CreateDisc(options);

  vertexData->applyToMesh(*disc, options.updatable);

  return disc;
}

} // end of namespace BABYLON
