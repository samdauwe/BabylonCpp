#include <babylon/samples/proceduraltextureslibrary/perlin_noise_procedural_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/perlinnoise/perlin_noise_procedural_texture.h>

namespace BABYLON {
namespace Samples {

PerlinNoiseProceduralTextureScene::PerlinNoiseProceduralTextureScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

PerlinNoiseProceduralTextureScene::~PerlinNoiseProceduralTextureScene()
{
}

const char* PerlinNoiseProceduralTextureScene::getName()
{
  return "Perlin Noise Procedural Texture Scene";
}

void PerlinNoiseProceduralTextureScene::initializeScene(ICanvas* canvas,
                                                        Scene* scene)
{
  auto camera = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2,
                                     25.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

  auto perlinnoiseMaterial
    = StandardMaterial::New("PerlinNoiseMaterial", scene);
  auto perlinnoiseTexture
    = ProceduralTexturesLibrary::PerlinNoiseProceduralTexture::New(
      "PerlinNoiseMaterialtext", 512, scene);
  perlinnoiseMaterial->setDiffuseTexture(perlinnoiseTexture);

  auto plane      = Mesh::CreatePlane("PerlinNoisePlane", 20, scene);
  plane->material = perlinnoiseMaterial;
  Vector3 axis(1.f, 1.f, 0.5f);
  plane->rotate(axis, Math::PI / 3.f, Space::LOCAL);
}

} // end of namespace Samples
} // end of namespace BABYLON
