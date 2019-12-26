#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/proceduraltextureslibrary/fire/fire_procedural_texture.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief FireProceduralTextureScene Scene. Example demonstrating how to use fire procedural
 * texture.
 * @see https://www.babylonjs-playground.com/#KM3TC
 * @see https://doc.babylonjs.com/extensions/FireProceduralTexture
 */
struct FireProceduralTextureScene : public IRenderableScene {

  FireProceduralTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~FireProceduralTextureScene() override = default;

  const char* getName() override
  {
    return "Fire Procedural Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2, 25.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);

    auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

    auto fireMaterial = StandardMaterial::New("fontainSculptur2", scene);
    auto fireTexture
      = ProceduralTexturesLibrary::FireProceduralTexture::New("FireMaterialtext", 256, scene);
    fireMaterial->diffuseTexture = fireTexture;
    fireMaterial->opacityTexture = fireTexture;
    fireMaterial->diffuseTexture = fireTexture;

    auto fireball = Mesh::CreatePlane("firebawl", 20, scene);

    fireball->material = fireMaterial;
    fireball->rotate(Vector3(1.f, 1.f, 0.5f), Math::PI / 3.f, Space::LOCAL);
  }

}; // end of struct FireProceduralTextureScene

BABYLON_REGISTER_SAMPLE("Procedural Textures Library", FireProceduralTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
