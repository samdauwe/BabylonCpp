#include <babylon/samples/lights/point_light_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

PointLightScene::PointLightScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

PointLightScene::~PointLightScene()
{
}

const char* PointLightScene::getName()
{
  return "Point Light Scene";
}

void PointLightScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light direction is directly down
  auto light      = PointLight::New("light", Vector3(0.f, 1.f, 0.f), scene);
  light->diffuse  = Color3(1.f, 0.f, 0.f);
  light->specular = Color3(0.f, 1.f, 0.f);

  SphereOptions options;
  options.diameter     = 1.5f;
  auto sphere          = MeshBuilder::CreateSphere("sphere", options, scene);
  sphere->position().z = 1.25f;
}

} // end of namespace Samples
} // end of namespace BABYLON
