#include <babylon/samples/textures/spherical_reflection_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

SphericalReflectionTextureScene::SphericalReflectionTextureScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

SphericalReflectionTextureScene::~SphericalReflectionTextureScene()
{
}

const char* SphericalReflectionTextureScene::getName()
{
  return "Spherical Reflection Texture Scene";
}

void SphericalReflectionTextureScene::initializeScene(ICanvas* canvas,
                                                      Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  scene->ambientColor = Color3(1.f, 1.f, 1.f);

  auto light
    = HemisphericLight::New("hemiLight", Vector3(-1.f, 10.f, -10.f), scene);
  light->diffuse   = Color3(1.f, 1.f, 1.f);
  light->intensity = 0.5f;

  auto mat = StandardMaterial::New("mat", scene);
  mat->diffuseTexture
    = Texture::New("textures/sphericalReflectionTexture.png", scene);

  auto sphericalMat = StandardMaterial::New("sphericalMat", scene);
  auto reflectionTexture
    = Texture::New("textures/sphericalReflectionTexture.png", scene);
  reflectionTexture->coordinatesMode = TextureConstants::SPHERICAL_MODE;
  sphericalMat->reflectionTexture    = reflectionTexture;

  auto planarMat = StandardMaterial::New("planarMat", scene);
  reflectionTexture
    = Texture::New("textures/sphericalReflectionTexture.png", scene);
  reflectionTexture->coordinatesMode = TextureConstants::PLANAR_MODE;
  planarMat->reflectionTexture       = reflectionTexture;

  auto projectionMat = StandardMaterial::New("projectionMat", scene);
  reflectionTexture
    = Texture::New("textures/sphericalReflectionTexture.png", scene);
  reflectionTexture->coordinatesMode = TextureConstants::PROJECTION_MODE;
  projectionMat->reflectionTexture   = reflectionTexture;

  SphereOptions options;
  auto sphere          = MeshBuilder::CreateSphere("sphere", options, scene);
  sphere->material     = mat;
  sphere->position().x = -1.f;
  sphere->position().y = 1.f;

  auto sphere1          = MeshBuilder::CreateSphere("sphere1", options, scene);
  sphere1->material     = sphericalMat;
  sphere1->position().x = 1.f;
  sphere1->position().y = 1.f;

  auto sphere2          = MeshBuilder::CreateSphere("sphere2", options, scene);
  sphere2->material     = planarMat;
  sphere2->position().x = -1.f;
  sphere2->position().y = -1.f;

  auto sphere3          = MeshBuilder::CreateSphere("sphere3", options, scene);
  sphere3->material     = projectionMat;
  sphere3->position().x = 1.f;
  sphere3->position().y = -1.f;
}

} // end of namespace Samples
} // end of namespace BABYLON
