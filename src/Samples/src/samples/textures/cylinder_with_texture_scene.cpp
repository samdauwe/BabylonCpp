#include <babylon/samples/textures/cylinder_with_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

CylinderWithTextureScene::CylinderWithTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

CylinderWithTextureScene::~CylinderWithTextureScene()
{
}

const char* CylinderWithTextureScene::getName()
{
  return "Cylinder With Texture Scene";
}

void CylinderWithTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New(
    "Camera", Math::PI_2, 7.f * Math::PI / 16.f, 2.5f, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  auto light1
    = HemisphericLight::New("hemiLight1", Vector3(-1.f, 10.f, -5.f), scene);
  light1->intensity = 0.97f;
  auto light2
    = HemisphericLight::New("hemiLight2", Vector3(-10.f, -10.f, -5.f), scene);
  light2->intensity = 0.97f;

  auto canMaterial            = StandardMaterial::New("material", scene);
  canMaterial->diffuseTexture = Texture::New("textures/cannedJam.jpg", scene);

  std::vector<Vector4> faceUV{
    Vector4(0.f, 0.f, 0.f, 0.f),   //
    Vector4(1.f, 0.f, 0.32f, 1.f), //
    Vector4(0.f, 0.f, 0.25f, 1.f)  //
  };

  std::vector<Color4> faceColors{Color4(0.5, 0.5, 0.5, 1)};

  CylinderOptions options;
  options.height     = 1.16f;
  options.faceUV     = std::move(faceUV);
  options.faceColors = std::move(faceColors);

  auto can      = MeshBuilder::CreateCylinder("can", options, scene);
  can->material = canMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
