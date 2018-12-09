#include <babylon/samples/textures/sphere_textures_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

SphereTexturesScene::SphereTexturesScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

SphereTexturesScene::~SphereTexturesScene()
{
}

const char* SphereTexturesScene::getName()
{
  return "Sphere Textures Scene";
}

void SphereTexturesScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_4, 5,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light direction is up and left
  auto light     = HemisphericLight::New("hemiLight", Vector3(-1, 1, 0), scene);
  light->diffuse = Color3(1.f, 0.f, 0.f);
  light->specular    = Color3(0.f, 1.f, 0.f);
  light->groundColor = Color3(0.f, 1.f, 0.f);

  auto grass0            = StandardMaterial::New("grass0", scene);
  grass0->diffuseTexture = Texture::New("textures/grass.png", scene);

  auto grass1             = StandardMaterial::New("grass1", scene);
  grass1->emissiveTexture = Texture::New("textures/grass.png", scene);

  auto grass2            = StandardMaterial::New("grass2", scene);
  grass2->ambientTexture = Texture::New("textures/grass.png", scene);
  grass2->diffuseColor   = Color3(1.f, 0.f, 0.f);

  // Diffuse texture
  SphereOptions sphereOptions;
  auto sphere0 = MeshBuilder::CreateSphere("sphere0", sphereOptions, scene);
  sphere0->position().x = -1.5f;
  sphere0->material     = grass0;

  // Emissive texture
  auto sphere1 = MeshBuilder::CreateSphere("sphere1", sphereOptions, scene);
  sphere1->material = grass1;

  // Ambient texture and diffuse color
  auto sphere2 = MeshBuilder::CreateSphere("sphere2", sphereOptions, scene);
  sphere2->material     = grass2;
  sphere2->position().x = 1.5f;
}

} // end of namespace Samples
} // end of namespace BABYLON
