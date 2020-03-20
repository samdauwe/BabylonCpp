#include <babylon/meshes/builders/ico_sphere_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr IcoSphereBuilder::CreateIcoSphere(const std::string& name, IcoSphereOptions& options,
                                          Scene* scene)
{
  auto sphere = Mesh::New(name, scene);

  options.sideOrientation = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  sphere->_originalBuilderSideOrientation = *options.sideOrientation;

  auto vertexData = VertexData::CreateIcoSphere(options);

  vertexData->applyToMesh(*sphere, options.updatable);

  return sphere;
}

} // end of namespace BABYLON
