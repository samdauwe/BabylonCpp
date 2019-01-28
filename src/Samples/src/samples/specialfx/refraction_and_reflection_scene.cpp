#include <babylon/samples/specialfx/refraction_and_reflection_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

RefractionAndReflectionScene::RefractionAndReflectionScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

RefractionAndReflectionScene::~RefractionAndReflectionScene()
{
}

const char* RefractionAndReflectionScene::getName()
{
  return "Refraction And Reflection Scene";
}

void RefractionAndReflectionScene::initializeScene(ICanvas* canvas,
                                                   Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
  auto material = StandardMaterial::New("kosh", scene);
  auto sphere1  = Mesh::CreateSphere("Sphere1", 32, 5, scene);
  auto light = PointLight::New("Omni0", Vector3(-17.6f, 18.8f, -49.9f), scene);

  camera->setPosition(Vector3(-15.f, 3.f, 0.f));
  camera->attachControl(canvas, true);

  // Sphere1 material
  material->refractionTexture
    = CubeTexture::New("textures/skybox/TropicalSunnyDay", scene);
  material->reflectionTexture
    = CubeTexture::New("textures/skybox/TropicalSunnyDay", scene);
  material->diffuseColor      = Color3(0.f, 0.f, 0.f);
  material->invertRefractionY = false;
  material->indexOfRefraction = 0.98f;
  material->specularPower     = 128.f;
  sphere1->material           = material;

  material->refractionFresnelParameters()->power      = 2.f;
  material->reflectionFresnelParameters()->power      = 2.f;
  material->reflectionFresnelParameters()->leftColor  = Color3::Black();
  material->reflectionFresnelParameters()->rightColor = Color3::White();

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
