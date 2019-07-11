#include <babylon/samples/meshes/is_point_inside_mesh_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/meshes/mesh_utils.h>

namespace BABYLON {
namespace Samples {

IsPointInsideMeshScene::IsPointInsideMeshScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

IsPointInsideMeshScene::~IsPointInsideMeshScene()
{
}

const char* IsPointInsideMeshScene::getName()
{
  return "Is Point Inside Mesh Scene";
}

void IsPointInsideMeshScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI / 8.f,
                                     20.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("hemi", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 0.98f;

  // Create 12 Pointed Star Mesh
  auto tps                   = MeshUtils::CreateTwelvePointedStar(3ull, scene);
  tps->material              = StandardMaterial::New("", scene);
  tps->material()->wireframe = true;

  // Inside mesh sphere
  SphereOptions options;
  options.diameter  = 0.2f;
  options.segments  = 6u;
  auto sphereInside = MeshBuilder::CreateSphere("pointInside", options, scene);
  auto sphereMaterial          = StandardMaterial::New("", scene);
  sphereMaterial->diffuseColor = Color3::Red();
  sphereInside->material       = sphereMaterial;

  // Outside mesh sphere
  auto sphereOutside
    = MeshBuilder::CreateSphere("pointOutside", options, scene);
  sphereOutside->material = StandardMaterial::New("", scene);

  for (size_t p = 0; p < 1000; ++p) {
    Vector3 point(-5.f + Math::random() * 10.f, -5.f + Math::random() * 10.f,
                  -5.f + Math::random() * 10.f);
    auto instance = (tps->pointIsInside(point)) ?
                      sphereInside->createInstance("") :
                      sphereOutside->createInstance("");
    instance->position = point;
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
