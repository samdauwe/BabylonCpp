#include <babylon/samples/lights/hemispheric_light_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

HemisphericLightScene::HemisphericLightScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

HemisphericLightScene::~HemisphericLightScene()
{
}

const char* HemisphericLightScene::getName()
{
  return "Hemispheric Light Scene";
}

void HemisphericLightScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light direction is up and left
  auto light
    = HemisphericLight::New("hemiLight", Vector3(-1.f, 1.f, 0.f), scene);
  light->diffuse     = Color3(1.f, 0.f, 0.f);
  light->specular    = Color3(0.f, 1.f, 0.f);
  light->groundColor = Color3(0.f, 1.f, 0.f);

  SphereOptions options;
  options.diameter = 1.5f;
  MeshBuilder::CreateSphere("sphere", options, scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
