#include <babylon/samples/materials/glossiness_and_roughness_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

GlossinessAndRoughnessScene::GlossinessAndRoughnessScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

GlossinessAndRoughnessScene::~GlossinessAndRoughnessScene()
{
}

const char* GlossinessAndRoughnessScene::getName()
{
  return "Glossiness and roughness scene";
}

void GlossinessAndRoughnessScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f,
                                     Vector3(0.f, 0.f, 0.f), scene);
  camera->attachControl(canvas, true);

  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

  auto sphere = Mesh::CreateSphere("sphere1", 16, 2, scene);

  auto mat             = StandardMaterial::New("toto", scene);
  mat->specularTexture = Texture::New("textures/specularglossymap.png", scene);
  mat->specularPower   = 64.f;
  mat->useGlossinessFromSpecularMapAlpha = true;
  mat->diffuseColor                      = Color3::Black();
  mat->roughness                         = 3.f;

  mat->reflectionTexture = CubeTexture::New("textures/skybox/skybox", scene);

  sphere->material = mat;

  // Skybox
  auto skybox                     = Mesh::CreateBox("skyBox", 100.f, scene);
  auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
  skyboxMaterial->backFaceCulling = false;
  skyboxMaterial->reflectionTexture
    = CubeTexture::New("textures/skybox/skybox", scene);
  skyboxMaterial->reflectionTexture()->coordinatesMode
    = TextureConstants::SKYBOX_MODE;
  skyboxMaterial->diffuseColor  = Color3(0.f, 0.f, 0.f);
  skyboxMaterial->specularColor = Color3(0.f, 0.f, 0.f);
  skybox->material              = skyboxMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
