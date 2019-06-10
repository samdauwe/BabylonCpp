#include <babylon/meshes/builders/disc_builder.h>

#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/meshes/vertex_data_options.h>

namespace BABYLON {

MeshPtr CreateDisc(const std::string& name, DiscOptions& options, Scene* scene)
{
  auto disc = Mesh::New(name, scene);

  options.sideOrientation
    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  disc->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateDisc(options);

  vertexData->applyToMesh(*disc, options.updatable);

  return disc;
}

} // end of namespace BABYLON
