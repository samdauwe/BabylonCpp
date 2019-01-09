#include <babylon/samples/textures/two_sided_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

#include <babylon/mesh/vertex_data.h>

namespace BABYLON {
namespace Samples {

TwoSidedTextureScene::TwoSidedTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

TwoSidedTextureScene::~TwoSidedTextureScene()
{
}

const char* TwoSidedTextureScene::getName()
{
  return "Two Sided Texture Scene";
}

void TwoSidedTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Add a camera to the scene and attach it to the canvas
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 4.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Add lights to the scene
  auto light1 = HemisphericLight::New("light1", Vector3(1.f, 1.f, 0.f), scene);
  light1->intensity = 0.97f;
  auto light2       = PointLight::New("light2", Vector3(0.f, 1.f, -1.f), scene);
  light2->intensity = 0.97f;

  // Add and manipulate meshes in the scene

  // Split image between sides
  // - front image = half the whole image along the width
  auto f = Vector4(0.5f, 0.f, 1.f, 1.f);
  // - back image = second half along the width
  auto b = Vector4(0.f, 0.f, 0.5f, 1.f);

  PlaneOptions options;
  options.height          = 1.f;
  options.width           = 0.665f;
  options.sideOrientation = Mesh::DOUBLESIDE;
  options.frontUVs        = f;
  options.backUVs         = b;
  auto plane              = MeshBuilder::CreatePlane("plane", options, scene);

  auto mat            = StandardMaterial::New("mat", scene);
  mat->diffuseTexture = Texture::New("textures/playingCard.jpg", scene);
  plane->material     = mat;
}

} // end of namespace Samples
} // end of namespace BABYLON
