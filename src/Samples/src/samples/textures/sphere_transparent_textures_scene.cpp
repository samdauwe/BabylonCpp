#include <babylon/samples/textures/sphere_transparent_textures_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

SphereTransparentTexturesScene::SphereTransparentTexturesScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

SphereTransparentTexturesScene::~SphereTransparentTexturesScene()
{
}

const char* SphereTransparentTexturesScene::getName()
{
  return "Sphere Transparent Textures Scene";
}

void SphereTransparentTexturesScene::initializeScene(ICanvas* canvas,
                                                     Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, 3 * Math::PI / 8.f,
                                     5, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light direction is up and left
  auto light
    = HemisphericLight::New("hemiLight", Vector3(-1.f, 1.f, 0.f), scene);
  light->diffuse     = Color3(1.f, 0.f, 0.f);
  light->specular    = Color3(0.f, 1.f, 0.f);
  light->groundColor = Color3(0.f, 1.f, 0.f);

  auto redMat            = StandardMaterial::New("redMat", scene);
  redMat->diffuseTexture = Texture::New("textures/grass.png", scene);

  auto greenMat            = StandardMaterial::New("greenMat", scene);
  greenMat->diffuseTexture = Texture::New("textures/grass.png", scene);
  greenMat->alpha          = 0.5f;

  // Red
  SphereOptions sphereOptions;
  auto sphere1 = MeshBuilder::CreateSphere("sphere1", sphereOptions, scene);
  sphere1->material     = redMat;
  sphere1->position().z = 1.5f;

  // Green Transparent
  auto sphere2 = MeshBuilder::CreateSphere("sphere2", sphereOptions, scene);
  sphere2->material = greenMat;
}

} // end of namespace Samples
} // end of namespace BABYLON
