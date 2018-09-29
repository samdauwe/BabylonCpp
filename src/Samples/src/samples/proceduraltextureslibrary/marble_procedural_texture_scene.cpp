#include <babylon/samples/proceduraltextureslibrary/marble_procedural_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/marble/marble_procedural_texture.h>

namespace BABYLON {
namespace Samples {

MarbleProceduralTextureScene::MarbleProceduralTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

MarbleProceduralTextureScene::~MarbleProceduralTextureScene()
{
}

const char* MarbleProceduralTextureScene::getName()
{
  return "Marble Procedural Texture Scene";
}

void MarbleProceduralTextureScene::initializeScene(ICanvas* canvas,
                                                   Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2,
                                     25.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

  auto marbleMaterial = StandardMaterial::New("MarbleMaterial", scene);
  auto marbleTexture  = ProceduralTexturesLibrary::MarbleProceduralTexture::New(
    "MarbleMaterialtext", 512, scene);
  marbleTexture->numberOfTilesHeight = 3.f;
  marbleTexture->numberOfTilesWidth  = 3.f;
  marbleMaterial->setDiffuseTexture(marbleTexture);

  auto plane      = Mesh::CreatePlane("MarblePlane", 20, scene);
  plane->material = marbleMaterial;
  Vector3 axis(1.f, 1.f, 0.5f);
  plane->rotate(axis, Math::PI / 3.f, Space::LOCAL);
}

} // end of namespace Samples
} // end of namespace BABYLON
