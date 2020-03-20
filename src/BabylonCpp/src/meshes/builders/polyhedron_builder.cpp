#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/polyhedron_builder.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr PolyhedronBuilder::CreatePolyhedron(const std::string& name, PolyhedronOptions& options,
                                            Scene* scene)
{
  auto polyhedron = Mesh::New(name, scene);

  options.sideOrientation = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  polyhedron->_originalBuilderSideOrientation = *options.sideOrientation;

  auto vertexData = VertexData::CreatePolyhedron(options);

  vertexData->applyToMesh(*polyhedron, options.updatable);

  return polyhedron;
}

} // end of namespace BABYLON
