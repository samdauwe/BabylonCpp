#include <babylon/meshes/builders/capsule_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr CapsuleBuilder::CreateCapsule(const std::string& name, ICreateCapsuleOptions& options,
                                      Scene* scene)
{
  auto capsule    = Mesh::New(name, scene);
  auto vertexData = VertexData::CreateCapsule(options);
  vertexData->applyToMesh(*capsule);
  return capsule;
}

} // end of namespace BABYLON
