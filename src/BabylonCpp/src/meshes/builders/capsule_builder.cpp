#include <babylon/meshes/builders/capsule_builder.h>

#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr CapsuleBuilder::CreateCapsule(const std::string& name, ICreateCapsuleOptions& options,
                                      Scene* scene)
{
  // Set default values
  {
    options.orientation     = options.orientation.value_or(Vector3::Up());
    options.subdivisions    = options.subdivisions.value_or(2u);
    options.tessellation    = options.tessellation.value_or(16u);
    options.height          = options.height.value_or(1.f);
    options.radius          = options.radius.value_or(0.25f);
    options.capSubdivisions = options.capSubdivisions.value_or(6u);
    options.updatable       = options.updatable.value_or(false);
  }

  const auto capsule    = Mesh::New(name, scene);
  const auto vertexData = VertexData::CreateCapsule(options);
  vertexData->applyToMesh(*capsule, options.updatable);
  return capsule;
}

} // end of namespace BABYLON
