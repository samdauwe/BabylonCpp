#include <babylon/samples/textures/bump_texture_spheres_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

BumpTextureSpheresScene::BumpTextureSpheresScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _sphere{nullptr}
{
}

BumpTextureSpheresScene::~BumpTextureSpheresScene()
{
}

const char* BumpTextureSpheresScene::getName()
{
  return "Bump Texture Spheres Scene";
}

void BumpTextureSpheresScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI / 4.f, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  auto light
    = HemisphericLight::New("hemiLight", Vector3(-1.f, 1.f, 0.f), scene);
  light->diffuse = Color3(1.f, 1.f, 1.f);

  // Material 0
  auto mat0          = StandardMaterial::New("mat0", scene);
  mat0->diffuseColor = Color3(1.f, 0.f, 0.f);
  mat0->bumpTexture  = Texture::New("textures/normalMap.png", scene);

  // Material 1
  auto mat1            = StandardMaterial::New("mat1", scene);
  mat1->diffuseTexture = Texture::New("textures/stone_bump.png", scene);
  mat1->bumpTexture    = Texture::New("textures/normalMap.png", scene);

  // Material 2
  auto mat2            = StandardMaterial::New("mat2", scene);
  mat2->diffuseTexture = Texture::New("textures/grass.png", scene);
  mat2->bumpTexture    = Texture::New("textures/normalMap.png", scene);

  // Sphere 0
  auto _sphere0          = Mesh::CreateSphere("Sphere0", 16, 1, scene);
  _sphere0->position().x = -1.5f;
  _sphere0->material     = mat0;

  // Sphere 1
  auto _sphere1      = Mesh::CreateSphere("Sphere1", 16, 1, scene);
  _sphere1->material = mat1;

  // Sphere 2
  auto _sphere2          = Mesh::CreateSphere("Sphere2", 16, 1, scene);
  _sphere2->position().x = 1.5f;
  _sphere2->material     = mat2;
}

} // end of namespace Samples
} // end of namespace BABYLON
