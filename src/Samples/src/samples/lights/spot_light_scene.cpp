#include <babylon/samples/lights/spot_light_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/spot_light.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>

namespace BABYLON {
namespace Samples {

SpotLightScene::SpotLightScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

SpotLightScene::~SpotLightScene()
{
}

const char* SpotLightScene::getName()
{
  return "Spot Light Scene";
}

void SpotLightScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light direction is directly down from a position one unit up, slow decay
  auto light      = SpotLight::New("spotLight", Vector3(-1.f, 1.f, -1.f),
                              Vector3(0.f, -1.f, 0.f), Math::PI_2, 10.f, scene);
  light->diffuse  = Color3(1.f, 0.f, 0.f);
  light->specular = Color3(0.f, 1.f, 0.f);

  // Light direction is directly down from a position one unit up, fast decay
  auto light1
    = SpotLight::New("spotLight1", Vector3(1.f, 1.f, 1.f),
                     Vector3(0.f, -1.f, 0.f), Math::PI_2, 50.f, scene);
  light1->diffuse  = Color3(0.f, 1.f, 0.f);
  light1->specular = Color3(0.f, 1.f, 0.f);

  GroundOptions options;
  options.width  = 4;
  options.height = 4;
  MeshBuilder::CreateGround("ground", options, scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
