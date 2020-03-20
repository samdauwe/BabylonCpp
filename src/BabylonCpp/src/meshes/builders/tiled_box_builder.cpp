#include <babylon/meshes/builders/tiled_box_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr TiledBoxBuilder::CreateTiledBox(const std::string& name, TiledBoxOptions& options,
                                        Scene* scene)
{
  auto box = Mesh::New(name, scene);

  options.sideOrientation              = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  box->_originalBuilderSideOrientation = *options.sideOrientation;

  auto vertexData = VertexData::CreateTiledBox(options);

  vertexData->applyToMesh(*box, options.updatable);

  return box;
}

} // end of namespace BABYLON
