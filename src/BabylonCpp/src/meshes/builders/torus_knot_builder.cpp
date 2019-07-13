#include <babylon/meshes/builders/torus_knot_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr TorusKnotBuilder::CreateTorusKnot(const std::string& name,
                                          TorusKnotOptions& options,
                                          Scene* scene)
{
  auto torusKnot = Mesh::New(name, scene);

  options.sideOrientation
    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  torusKnot->_originalBuilderSideOrientation = *options.sideOrientation;

  auto vertexData = VertexData::CreateTorusKnot(options);

  vertexData->applyToMesh(*torusKnot, options.updatable);

  return torusKnot;
}

} // end of namespace BABYLON
