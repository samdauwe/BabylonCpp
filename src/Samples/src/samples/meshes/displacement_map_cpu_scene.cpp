#include <babylon/samples/meshes/displacement_map_cpu_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

DisplacementMapCPUScene::DisplacementMapCPUScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

DisplacementMapCPUScene::~DisplacementMapCPUScene()
{
}

const char* DisplacementMapCPUScene::getName()
{
  return "Displacement map (CPU) Scene";
}

void DisplacementMapCPUScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 0, 0, 10, Vector3::Zero(), scene);
  auto light = HemisphericLight::New("Omni0", Vector3(0.f, 1.f, 0.f), scene);
  light->intensity = 0.98f;
  auto material    = StandardMaterial::New("kosh", scene);
  auto sphere      = Mesh::CreateSphere("Sphere", 32, 3, scene, true);

  camera->setPosition(Vector3(-10, 10, 0));
  camera->attachControl(canvas, true);

  sphere->applyDisplacementMap("/textures/amiga.jpg", 0, 1.5f);

  // Sphere material
  material->diffuseTexture = Texture::New("/textures/amiga.jpg", scene);
  sphere->material         = material;

  // Skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 100.0, scene);
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
}

} // end of namespace Samples
} // end of namespace BABYLON
