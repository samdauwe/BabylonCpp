#include <babylon/samples/lights/simultaneous_lights_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/string.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

SimultaneousLightsScene::SimultaneousLightsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _light0{nullptr}
    , _light1{nullptr}
    , _light2{nullptr}
    , _light3{nullptr}
    , _lightSphere0{nullptr}
    , _lightSphere1{nullptr}
    , _lightSphere2{nullptr}
    , _alpha{0.f}
{
}

SimultaneousLightsScene::~SimultaneousLightsScene()
{
}

const char* SimultaneousLightsScene::getName()
{
  return "Simultaneous Lights Scene";
}

void SimultaneousLightsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
  _light0       = PointLight::New("Omni0", Vector3(0.f, 10.f, 0.f), scene);
  _light1       = PointLight::New("Omni1", Vector3(0.f, -10.f, 0.f), scene);
  _light2       = PointLight::New("Omni2", Vector3(10.f, 0.f, 0.f), scene);
  _light3       = DirectionalLight::New("Dir0", Vector3(1.f, -1.f, 0.f), scene);
  auto material = StandardMaterial::New("kosh", scene);
  auto sphere   = Mesh::CreateSphere("Sphere", 16, 3, scene);

  camera->setPosition(Vector3(-10.f, 10.f, 0.f));
  camera->attachControl(canvas, true);

  // Creating light sphere
  _lightSphere0 = Mesh::CreateSphere("Sphere0", 16, 0.5f, scene);
  _lightSphere1 = Mesh::CreateSphere("Sphere1", 16, 0.5f, scene);
  _lightSphere2 = Mesh::CreateSphere("Sphere2", 16, 0.5f, scene);

  auto mat0               = StandardMaterial::New("red", scene);
  mat0->diffuseColor      = Color3(0, 0, 0);
  mat0->specularColor     = Color3(0, 0, 0);
  mat0->emissiveColor     = Color3(1, 0, 0);
  _lightSphere0->material = mat0;

  auto mat1               = StandardMaterial::New("green", scene);
  mat1->diffuseColor      = Color3(0, 0, 0);
  mat1->specularColor     = Color3(0, 0, 0);
  mat1->emissiveColor     = Color3(0, 1, 0);
  _lightSphere1->material = mat1;

  auto mat2               = StandardMaterial::New("blue", scene);
  mat2->diffuseColor      = Color3(0, 0, 0);
  mat2->specularColor     = Color3(0, 0, 0);
  mat2->emissiveColor     = Color3(0, 0, 1);
  _lightSphere2->material = mat2;

  // Sphere material
  material->diffuseColor = Color3(1, 1, 1);
  sphere->material       = material;

  // Lights colors
  _light0->diffuse  = Color3(1, 0, 0);
  _light0->specular = Color3(1, 0, 0);

  _light1->diffuse  = Color3(0, 1, 0);
  _light1->specular = Color3(0, 1, 0);

  _light2->diffuse  = Color3(0, 0, 1);
  _light2->specular = Color3(0, 0, 1);

  _light3->diffuse  = Color3(1, 1, 1);
  _light3->specular = Color3(1, 1, 1);

  // Skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 100.f, scene);
  auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  skyboxMaterial->reflectionTexture
    = CubeTexture::New("/textures/skybox/skybox", scene);
  skyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->diffuseColor    = Color3(0, 0, 0);
  skyboxMaterial->specularColor   = Color3(0, 0, 0);
  skyboxMaterial->disableLighting = true;
  skybox->material                = skyboxMaterial;
  skybox->infiniteDistance        = true;

  // Animations
  scene->onBeforeRenderObservable.add([this](Scene*, EventState&) {
    _light0->position
      = Vector3(10.f * std::sin(_alpha), 0.f, 10.f * std::cos(_alpha));
    _light1->position
      = Vector3(10.f * std::sin(_alpha), 0.f, -10.f * std::cos(_alpha));
    _light2->position
      = Vector3(10.f * std::cos(_alpha), 0.f, 10.f * std::sin(_alpha));

    _lightSphere0->position = _light0->position();
    _lightSphere1->position = _light1->position();
    _lightSphere2->position = _light2->position();

    _alpha += 0.01f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
