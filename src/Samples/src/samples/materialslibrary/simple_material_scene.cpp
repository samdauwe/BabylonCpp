#include <babylon/samples/materialslibrary/simple_material_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materialslibrary/simple/simple_material.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

SimpleMaterialScene::SimpleMaterialScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

SimpleMaterialScene::~SimpleMaterialScene()
{
}

const char* SimpleMaterialScene::getName()
{
  return "Simple Material Scene";
}

void SimpleMaterialScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = FreeCamera::New("camera1", Vector3(5.f, 4.f, -47.f), scene);
  camera->setTarget(Vector3::Zero());
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 1.f;

  auto ground = Mesh::CreateGroundFromHeightMap(
    "ground", "textures/heightMap.png", 100, 100, 100, 0, 10, scene, false);

  // Create SimpleMaterial
  auto groundMaterial = MaterialsLibrary::SimpleMaterial::New("ground", scene);
  groundMaterial->diffuseTexture = Texture::New("textures/ground.jpg", scene);
  groundMaterial->diffuseTexture()->uScale = 6;
  groundMaterial->diffuseTexture()->vScale = 6;
  ground->position().y                     = -2.05f;
  ground->material                         = groundMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
