#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/proceduraltextureslibrary/starfield/starfield_procedural_texture.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief StarfieldProceduralTextureScene Scene. Example demonstrating how to use starfield
 * procedural texture.
 */
class StarfieldProceduralTextureScene : public IRenderableScene {

public:
  StarfieldProceduralTextureScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _starfieldTexture{nullptr}
  {
  }

  ~StarfieldProceduralTextureScene() override = default;

  const char* getName() override
  {
    return "Star Field Procedural Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", 0.f, 0.f, -30.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);

    auto light0 = HemisphericLight::New("Light", Vector3(0.f, 0.5f, 0.f), scene);

    auto starfieldMaterial = StandardMaterial::New("StarFieldMaterial", scene);
    _starfieldTexture      = ProceduralTexturesLibrary::StarfieldProceduralTexture::New(
      "StarFieldMaterialtext", 512, scene);
    _starfieldTexture->beta            = 0.1f;
    _starfieldTexture->coordinatesMode = TextureConstants::SKYBOX_MODE;
    starfieldMaterial->diffuseTexture  = _starfieldTexture;
    starfieldMaterial->backFaceCulling = false;

    auto spaceScale = 10.f;
    auto space
      = Mesh::CreateCylinder("Space", 10.f * spaceScale, 0, 6.f * spaceScale, 20, 20, scene);
    space->material = starfieldMaterial;

    scene->registerBeforeRender([this](Scene* scene, EventState&) {
      _starfieldTexture->time = _starfieldTexture->time() + scene->getAnimationRatio() * 0.8f;
    });
  }

private:
  ProceduralTexturesLibrary::StarfieldProceduralTexturePtr _starfieldTexture;

}; // end of class StarfieldProceduralTextureScene

BABYLON_REGISTER_SAMPLE("Procedural Textures Library", StarfieldProceduralTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
