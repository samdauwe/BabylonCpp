#include <babylon/samples/specialfx/fog_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

FogScene::FogScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _alpha{0.f}
    , _sphere0{nullptr}
    , _sphere1{nullptr}
    , _sphere2{nullptr}
{
}

FogScene::~FogScene()
{
}

const char* FogScene::getName()
{
  return "Fog Scene";
}

void FogScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a FreeCamera (non-mesh)
  auto camera = FreeCamera::New("Camera", Vector3(0.f, 0.f, -20.f), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a point light
  PointLight::New("Omni", Vector3(20.f, 100.f, 2.f), scene);

  // Create spheres
  _sphere0 = Mesh::CreateSphere("Sphere0", 16, 3.f, scene);
  _sphere1 = Mesh::CreateSphere("Sphere1", 16, 3.f, scene);
  _sphere2 = Mesh::CreateSphere("Sphere2", 16, 3.f, scene);

  // Create materials
  auto material0          = StandardMaterial::New("mat0", scene);
  material0->diffuseColor = Color3(1.f, 0.f, 0.f);
  _sphere0->material      = material0;
  _sphere0->setPosition(Vector3(-10.f, 0.f, 0.f));

  auto material1          = StandardMaterial::New("mat1", scene);
  material1->diffuseColor = Color3(1.f, 1.f, 0.f);
  _sphere1->material      = material1;

  auto material2          = StandardMaterial::New("mat2", scene);
  material2->diffuseColor = Color3(1.f, 0.f, 1.f);
  _sphere2->material      = material2;
  _sphere2->setPosition(Vector3(10.f, 0.f, 0.f));

  _sphere1->convertToFlatShadedMesh();

  // Fog settings
  scene->setFogMode(Scene::FOGMODE_EXP());
  scene->fogDensity = 0.1f;

  // Animations
  _scene->registerBeforeRender([this](Scene*, EventState&) {
    _sphere0->position().z = 4.f * std::cos(_alpha);
    _sphere1->position().z = 4.f * std::sin(_alpha);
    _sphere2->position().z = 4.f * std::cos(_alpha);

    _alpha += 0.1f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
