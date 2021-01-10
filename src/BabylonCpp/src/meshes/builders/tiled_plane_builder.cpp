#include <babylon/meshes/builders/tiled_plane_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr TiledPlaneBuilder::CreateTiledPlane(const std::string& name, TiledPlaneOptions& options,
                                            Scene* scene)
{
  const auto plane = Mesh::New(name, scene);

  options.sideOrientation = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  plane->_originalBuilderSideOrientation = *options.sideOrientation;

  const auto vertexData = VertexData::CreateTiledPlane(options);

  vertexData->applyToMesh(*plane, options.updatable);

  return plane;
}

} // end of namespace BABYLON
