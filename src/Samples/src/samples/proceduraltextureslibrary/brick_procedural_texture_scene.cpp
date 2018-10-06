#include <babylon/samples/proceduraltextureslibrary/brick_procedural_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/brick/brick_procedural_texture.h>

namespace BABYLON {
namespace Samples {

BrickProceduralTextureScene::BrickProceduralTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

BrickProceduralTextureScene::~BrickProceduralTextureScene()
{
}

const char* BrickProceduralTextureScene::getName()
{
  return "Brick Procedural Texture Scene";
}

void BrickProceduralTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", 2.8f * Math::PI_2, Math::PI_2,
                                     15.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

  auto brickMaterial = StandardMaterial::New("BrickMaterial", scene);
  auto brickTexture  = ProceduralTexturesLibrary::BrickProceduralTexture::New(
    "BrickMaterialtext", 512, scene);
  brickTexture->numberOfBricksHeight = 6.f;
  brickTexture->numberOfBricksWidth  = 10.f;
  brickMaterial->diffuseTexture      = brickTexture;

  auto plane      = Mesh::CreatePlane("BrickPlane", 10, scene);
  plane->material = brickMaterial;
}

} // end of namespace Samples
} // end of namespace BABYLON
