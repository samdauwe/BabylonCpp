#include <babylon/samples/specialfx/fresnel_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

FresnelScene::FresnelScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

FresnelScene::~FresnelScene()
{
}

const char* FresnelScene::getName()
{
  return "Fresnel Scene";
}

void FresnelScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
  auto material = StandardMaterial::New("kosh", scene);
  auto sphere1  = Mesh::CreateSphere("Sphere1", 32, 3, scene);
  auto sphere2  = Mesh::CreateSphere("Sphere2", 32, 3, scene);
  auto sphere3  = Mesh::CreateSphere("Sphere3", 32, 3, scene);
  auto sphere4  = Mesh::CreateSphere("Sphere4", 32, 3, scene);
  auto sphere5  = Mesh::CreateSphere("Sphere5", 32, 3, scene);
  auto light = PointLight::New("Omni0", Vector3(-17.6f, 18.8f, -49.9f), scene);
  light->intensity = 0.97f;

  camera->setPosition(Vector3(-15.f, 3.f, 0.f));
  camera->attachControl(canvas, true);

  sphere2->position().z -= 5.f;
  sphere3->position().z += 5.f;
  sphere4->position().x += 5.f;
  sphere5->position().x -= 5.f;

  // Sphere1 material
  material->reflectionTexture
    = CubeTexture::New("textures/skybox/TropicalSunnyDay", scene);
  material->diffuseColor  = Color3(0.f, 0.f, 0.f);
  material->emissiveColor = Color3(0.5f, 0.5f, 0.5f);
  material->alpha         = 0.2f;
  material->specularPower = 16.f;

  // Fresnel
  material->reflectionFresnelParameters()->bias = 0.1f;

  material->emissiveFresnelParameters()->bias       = 0.6f;
  material->emissiveFresnelParameters()->power      = 4.f;
  material->emissiveFresnelParameters()->leftColor  = Color3::White();
  material->emissiveFresnelParameters()->rightColor = Color3::Black();

  material->opacityFresnelParameters()->leftColor  = Color3::White();
  material->opacityFresnelParameters()->rightColor = Color3::Black();

  sphere1->material = material;

  // Sphere2 material
  material = StandardMaterial::New("kosh2", scene);
  material->reflectionTexture
    = CubeTexture::New("textures/skybox/TropicalSunnyDay", scene);
  material->diffuseColor  = Color3(0.f, 0.f, 0.f);
  material->emissiveColor = Color3(0.5f, 0.5f, 0.5f);
  material->specularPower = 32.f;

  // Fresnel
  material->reflectionFresnelParameters()->bias = 0.1f;

  material->emissiveFresnelParameters()->bias       = 0.5f;
  material->emissiveFresnelParameters()->power      = 4.f;
  material->emissiveFresnelParameters()->leftColor  = Color3::White();
  material->emissiveFresnelParameters()->rightColor = Color3::Black();

  sphere2->material = material;

  // Sphere3 material
  material                = StandardMaterial::New("kosh3", scene);
  material->diffuseColor  = Color3(0.f, 0.f, 0.f);
  material->emissiveColor = Color3::White();
  material->specularPower = 64.f;
  material->alpha         = 0.2f;

  // Fresnel
  material->emissiveFresnelParameters()->bias       = 0.2f;
  material->emissiveFresnelParameters()->leftColor  = Color3::White();
  material->emissiveFresnelParameters()->rightColor = Color3::Black();

  material->opacityFresnelParameters()->power      = 4.f;
  material->opacityFresnelParameters()->leftColor  = Color3::White();
  material->opacityFresnelParameters()->rightColor = Color3::Black();

  sphere3->material = material;

  // Sphere4 material
  material                = StandardMaterial::New("kosh4", scene);
  material->diffuseColor  = Color3(0, 0, 0);
  material->emissiveColor = Color3::White();
  material->specularPower = 64;

  // Fresnel
  material->emissiveFresnelParameters()->power      = 4.f;
  material->emissiveFresnelParameters()->bias       = 0.5f;
  material->emissiveFresnelParameters()->leftColor  = Color3::White();
  material->emissiveFresnelParameters()->rightColor = Color3::Black();

  sphere4->material = material;

  // Sphere5 material
  material               = StandardMaterial::New("kosh5", scene);
  material->diffuseColor = Color3(0, 0, 0);
  material->reflectionTexture
    = CubeTexture::New("textures/skybox/TropicalSunnyDay", scene);
  material->reflectionTexture()->level = 0.5f;
  material->specularPower              = 64.f;
  material->emissiveColor              = Color3(0.2f, 0.2f, 0.2f);

  // Fresnel
  material->emissiveFresnelParameters()->bias       = 0.4f;
  material->emissiveFresnelParameters()->power      = 2.f;
  material->emissiveFresnelParameters()->leftColor  = Color3::Black();
  material->emissiveFresnelParameters()->rightColor = Color3::White();

  sphere5->material = material;

  // Skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 100.f, scene);
  auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  skyboxMaterial->reflectionTexture
    = CubeTexture::New("textures/skybox/TropicalSunnyDay", scene);
  skyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->diffuseColor    = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->specularColor   = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->disableLighting = true;
  skybox->material                = skyboxMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
