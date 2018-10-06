#include <babylon/samples/proceduraltextureslibrary/wood_procedural_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/wood/wood_procedural_texture.h>

namespace BABYLON {
namespace Samples {

WoodProceduralTextureScene::WoodProceduralTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

WoodProceduralTextureScene::~WoodProceduralTextureScene()
{
}

const char* WoodProceduralTextureScene::getName()
{
  return "Wood Procedural Texture Scene";
}

void WoodProceduralTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2,
                                     12.5f, Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

  auto woodMaterial = StandardMaterial::New("WoodMaterial", scene);
  auto woodTexture  = ProceduralTexturesLibrary::WoodProceduralTexture::New(
    "WoodMaterialtext", 1024, scene);
  woodTexture->ampScale        = 80.f;
  woodMaterial->diffuseTexture = woodTexture;

  auto plane      = Mesh::CreatePlane("WoodPlane", 10, scene);
  plane->material = woodMaterial;
  Vector3 axis(1.f, 1.f, 0.5f);
  plane->rotate(axis, Math::PI / 3.f, Space::LOCAL);
}

} // end of namespace Samples
} // end of namespace BABYLON
