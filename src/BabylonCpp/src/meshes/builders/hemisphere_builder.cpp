#include <babylon/meshes/builders/hemisphere_builder.h>

#include <babylon/meshes/builders/disc_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

MeshPtr HemisphereBuilder::CreateHemisphere(const std::string& name,
                                            HemisphereOptions& options,
                                            Scene* scene)
{
  if (!options.diameter) {
    options.diameter = 1;
  }
  if (!options.segments) {
    options.segments = 16;
  }

  SphereOptions sphereOptions;
  sphereOptions.slice    = 0.5f;
  sphereOptions.diameter = *options.diameter;
  sphereOptions.segments = *options.segments;
  auto halfSphere = SphereBuilder::CreateSphere("", sphereOptions, scene);
  auto disc       = Mesh::CreateDisc(
    "", *options.diameter / 2.f,
    (*options.segments * 3) + (4 - *options.segments), scene);
  disc->rotation().x = -Math::PI_2;
  disc->parent       = halfSphere.get();

  auto merged  = Mesh::MergeMeshes({disc, halfSphere}, true);
  merged->name = name;

  return merged;
}

} // end of namespace BABYLON
