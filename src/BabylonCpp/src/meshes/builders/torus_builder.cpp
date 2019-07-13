#include <babylon/meshes/builders/torus_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr TorusBuilder::CreateTorus(const std::string& name,
                                  TorusOptions& options, Scene* scene)
{
  auto torus = Mesh::New(name, scene);

  options.sideOrientation
    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  torus->_originalBuilderSideOrientation = *options.sideOrientation;

  auto vertexData = VertexData::CreateTorus(options);

  vertexData->applyToMesh(*torus, options.updatable);

  return torus;
}

} // end of namespace BABYLON
