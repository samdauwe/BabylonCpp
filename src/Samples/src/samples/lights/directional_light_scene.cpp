#include <babylon/samples/lights/directional_light_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/directional_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

DirectionalLightScene::DirectionalLightScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

DirectionalLightScene::~DirectionalLightScene()
{
}

const char* DirectionalLightScene::getName()
{
  return "Directional Light Scene";
}

void DirectionalLightScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light direction is directly down
  auto light
    = DirectionalLight::New("DirectionalLight", Vector3(0.f, -1.f, 0.f), scene);
  light->diffuse  = Color3(1.f, 0.f, 0.f);
  light->specular = Color3(0.f, 1.f, 0.f);

  SphereOptions options;
  options.diameter = 1.5f;
  MeshBuilder::CreateSphere("sphere", options, scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
