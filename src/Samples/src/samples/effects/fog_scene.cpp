#include <babylon/samples/effects/fog_scene.h>

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
  // Create a FreeCamera (non-mesh), and set its position to (x:0, y:5, z:-30)
  auto camera = FreeCamera::New("camera1", Vector3(0, 5, -20), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a point light
  PointLight::New("light1", Vector3(20.f, 100.f, 2.f), scene);

  // Create spheres
  _sphere0 = Mesh::CreateSphere("sphere0", 16, 3.f, scene);
  _sphere1 = Mesh::CreateSphere("sphere1", 16, 3.f, scene);
  _sphere2 = Mesh::CreateSphere("sphere2", 16, 3.f, scene);

  // Create materials
  auto material0          = StandardMaterial::New("mat0", scene);
  material0->diffuseColor = Color3(1.f, 0.f, 0.f);
  _sphere0->setMaterial(material0);
  _sphere0->setPosition(Vector3(-10.f, 0.f, 0.f));

  auto material1          = StandardMaterial::New("mat1", scene);
  material1->diffuseColor = Color3(1.f, 1.f, 0.f);
  _sphere1->setMaterial(material1);

  auto material2          = StandardMaterial::New("mat2", scene);
  material2->diffuseColor = Color3(1.f, 0.f, 1.f);
  _sphere2->setMaterial(material2);
  _sphere2->setPosition(Vector3(10.f, 0.f, 0.f));

  _sphere1->convertToFlatShadedMesh();

  // Fog settings
  scene->setFogMode(Scene::FOGMODE_EXP);
  scene->fogDensity = 0.1f;

  // Animations
  _scene->registerBeforeRender([this]() {
    _sphere0->position().z = 4.f * std::cos(_alpha);
    _sphere1->position().z = 10.f * std::sin(_alpha);
    _sphere2->position().z = 4.f * std::cos(_alpha);

    _alpha += 0.08f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
