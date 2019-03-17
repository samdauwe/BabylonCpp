#include <babylon/samples/shadows/box_shadow_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {
namespace Samples {

BoxShadowScene::BoxShadowScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

BoxShadowScene::~BoxShadowScene()
{
}

const char* BoxShadowScene::getName()
{
  return "Box Shadow Scene";
}

void BoxShadowScene::initializeScene(ICanvas* canvas, Scene* scene)
{

  // Box
  auto box      = Mesh::CreateBox("Box", 6, scene);
  box->position = Vector3(6.f, -3.f, 5.f);

  // Light
  auto light = PointLight::New("pointLight", Vector3(-1.f, -1.f, -1.f), scene);
  light->position  = Vector3(20.f, 40.f, 20.f);
  light->intensity = 1.f;

  // Sphere
  auto lightSphere         = Mesh::CreateSphere("sphere", 10, 2, scene);
  lightSphere->position    = light->position();
  auto lightSphereMaterial = StandardMaterial::New("light", scene);
  lightSphereMaterial->emissiveColor = Color3(1.f, 1.f, 0.f);
  lightSphere->material              = lightSphereMaterial;

  auto material2          = StandardMaterial::New("std", scene);
  material2->diffuseColor = Color3(0.5f, 1.f, 0.5f);

  box->material = material2;

  // Shadows
  auto shadowGenerator = ShadowGenerator::New(1024, light);
  shadowGenerator->getShadowMap()->renderList().emplace_back(box.get());
  shadowGenerator->useBlurExponentialShadowMap = true;
  shadowGenerator->useKernelBlur               = true;
  shadowGenerator->blurKernel                  = 64.f;

  // Ground
  auto ground = Mesh::CreateGround("ground", 100, 100, 1, scene, false);
  ground->position().y   = -6.f;
  ground->receiveShadows = true;

  // Camera
  auto camera = ArcRotateCamera::New("arcCam", 0.f, Tools::ToRadians(45.f),
                                     60.f, box->position(), scene);
  camera->attachControl(canvas, true);
}

} // end of namespace Samples
} // end of namespace BABYLON
