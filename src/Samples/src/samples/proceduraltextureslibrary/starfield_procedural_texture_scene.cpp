#include <babylon/samples/proceduraltextureslibrary/starfield_procedural_texture_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/proceduraltextureslibrary/starfield/starfield_procedural_texture.h>

namespace BABYLON {
namespace Samples {

StarfieldProceduralTextureScene::StarfieldProceduralTextureScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _starfieldTexture{nullptr}
{
}

StarfieldProceduralTextureScene::~StarfieldProceduralTextureScene()
{
}

const char* StarfieldProceduralTextureScene::getName()
{
  return "Star Field Procedural Texture Scene";
}

void StarfieldProceduralTextureScene::initializeScene(ICanvas* canvas,
                                                      Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, -30.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, false);

  auto light0 = HemisphericLight::New("Light", Vector3(0.f, 0.5f, 0.f), scene);

  auto starfieldMaterial = StandardMaterial::New("StarFieldMaterial", scene);
  _starfieldTexture
    = ProceduralTexturesLibrary::StarfieldProceduralTexture::New(
      "StarFieldMaterialtext", 512, scene);
  _starfieldTexture->beta            = 0.1f;
  _starfieldTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
  starfieldMaterial->setDiffuseTexture(_starfieldTexture);
  starfieldMaterial->backFaceCulling = false;

  auto spaceScale = 10.f;
  auto space      = Mesh::CreateCylinder("Space", 10.f * spaceScale, 0,
                                    6.f * spaceScale, 20, 20, scene);
  space->material = starfieldMaterial;

  scene->registerBeforeRender([this](Scene* scene, EventState&) {
    _starfieldTexture->time
      = _starfieldTexture->time() + scene->getAnimationRatio() * 0.8f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
