#include <babylon/samples/proceduraltextureslibrary/grass_procedural_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/grass/grass_procedural_texture.h>

namespace BABYLON {
namespace Samples {

GrassProceduralTextureScene::GrassProceduralTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

GrassProceduralTextureScene::~GrassProceduralTextureScene()
{
}

const char* GrassProceduralTextureScene::getName()
{
  return "Grass Procedural Texture Scene";
}

void GrassProceduralTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2,
                                     12.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

  auto grassMaterial = StandardMaterial::New("GrassMaterial", scene);
  auto grassTexture  = ProceduralTexturesLibrary::GrassProceduralTexture::New(
    "GrassMaterialtext", 256, scene);
  grassMaterial->setAmbientTexture(grassTexture);

  auto plane      = Mesh::CreatePlane("GrassPlane", 10, scene);
  plane->material = grassMaterial;
  Vector3 axis(1.f, 1.f, 0.5f);
  plane->rotate(axis, Math::PI / 3.f, Space::LOCAL);
}

} // end of namespace Samples
} // end of namespace BABYLON
