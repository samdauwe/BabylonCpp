#include <babylon/samples/textures/opacity_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

OpacityTexturScene::OpacityTexturScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

OpacityTexturScene::~OpacityTexturScene()
{
}

const char* OpacityTexturScene::getName()
{
  return "Opacity Textur Scene";
}

void OpacityTexturScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Camera
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light direction is up and left
  auto light     = HemisphericLight::New("hemiLight", Vector3(-1, 1, 0), scene);
  light->diffuse = Color3::White();

  auto mat0          = StandardMaterial::New("mat0", scene);
  mat0->diffuseColor = Color3::Red();
  mat0->opacityTexture
    = Texture::New("textures/transparencyGradient.png", scene);

  auto mat1          = StandardMaterial::New("mat1", scene);
  mat1->diffuseColor = Color3::Magenta();

  // Plane with opacity material
  PlaneOptions planeOptions;
  auto plane      = MeshBuilder::CreatePlane("plane", planeOptions, scene);
  plane->material = mat0;

  // Shere
  SphereOptions sphereOptions;
  auto sphere1 = MeshBuilder::CreateSphere("sphere1", sphereOptions, scene);
  sphere1->material     = mat1;
  sphere1->position().z = 1.5f;
}

} // end of namespace Samples
} // end of namespace BABYLON
