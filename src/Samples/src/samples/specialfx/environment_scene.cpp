#include <babylon/samples/specialfx/environment_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

EnvironmentScene::EnvironmentScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _alpha{0.f}
{
}

EnvironmentScene::~EnvironmentScene()
{
}

const char* EnvironmentScene::getName()
{
  return "Environment Scene";
}

void EnvironmentScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto light       = PointLight::New("Omni", Vector3(10.f, 50.f, 50.f), scene);
  light->intensity = 0.97f;
  auto camera
    = ArcRotateCamera::New("Camera", 0.4f, 1.2f, 20.f, Vector3::Zero(), scene);
  camera->setTarget(Vector3(-10.f, 0.f, 0.f));

  camera->attachControl(canvas, true);

  auto material1          = StandardMaterial::New("mat1", scene);
  material1->diffuseColor = Color3(1.f, 1.f, 0.f);

  for (int i = 0; i < 10; ++i) {
    auto box      = Mesh::CreateBox("Box", 1.f, scene);
    box->material = material1;
    box->position = Vector3(-i * 5.f, 0.f, 0.f);
  }

  // Fog
  scene->fogMode = Scene::FOGMODE_EXP;
  // scene->fogMode = Scene::FOGMODE_NONE;
  // scene->fogMode = Scene::FOGMODE_EXP;
  // scene->fogMode = Scene::FOGMODE_EXP2;
  // scene->fogMode = Scene::FOGMODE_LINEAR;

  scene->fogColor   = Color3(0.9f, 0.9f, 0.85f);
  scene->fogDensity = 0.01f;

  // Only if LINEAR
  // scene->fogStart = 20.f;
  // scene->fogEnd = 60.f;

  // Skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 100.f, scene);
  auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  skyboxMaterial->reflectionTexture
    = CubeTexture::New("textures/skybox/skybox", scene);
  skyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->diffuseColor    = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->specularColor   = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->disableLighting = true;
  skybox->material                = skyboxMaterial;

  // Animations
  _scene->registerBeforeRender([this](Scene* scene, EventState& /*es*/) {
    scene->fogDensity = std::cos(_alpha) / 10.f;
    _alpha += 0.02f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
