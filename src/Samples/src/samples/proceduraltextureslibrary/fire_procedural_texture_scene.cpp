#include <babylon/samples/proceduraltextureslibrary/fire_procedural_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/fire/fire_procedural_texture.h>

namespace BABYLON {
namespace Samples {

FireProceduralTextureScene::FireProceduralTextureScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

FireProceduralTextureScene::~FireProceduralTextureScene()
{
}

const char* FireProceduralTextureScene::getName()
{
  return "Fire Procedural Texture Scene";
}

void FireProceduralTextureScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2,
                                     25.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

  auto fireMaterial = StandardMaterial::New("fontainSculptur2", scene);
  auto fireTexture  = ProceduralTexturesLibrary::FireProceduralTexture::New(
    "FireMaterialtext", 256, scene);
  fireMaterial->diffuseTexture = fireTexture;
  fireMaterial->opacityTexture = fireTexture;
  fireMaterial->diffuseTexture = fireTexture;

  auto fireball = Mesh::CreatePlane("firebawl", 20, scene);

  fireball->material = fireMaterial;
  Vector3 axis(1.f, 1.f, 0.5f);
  fireball->rotate(axis, Math::PI / 3.f, Space::LOCAL);
}

} // end of namespace Samples
} // end of namespace BABYLON
