#include <babylon/meshes/builders/sphere_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr SphereBuilder::CreateSphere(const std::string& name, SphereOptions& options, Scene* scene)
{
  const auto sphere = Mesh::New(name, scene);

  options.sideOrientation = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  sphere->_originalBuilderSideOrientation = *options.sideOrientation;

  const auto vertexData = VertexData::CreateSphere(options);

  vertexData->applyToMesh(*sphere, options.updatable);

  return sphere;
}

} // end of namespace BABYLON
