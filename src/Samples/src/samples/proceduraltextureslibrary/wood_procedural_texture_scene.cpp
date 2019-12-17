#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/proceduraltextureslibrary/wood/wood_procedural_texture.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief WoodProceduralTextureScene Scene. Example demonstrating how to use wood procedural
 * texture.
 * @see https://www.babylonjs-playground.com/#K41IJ
 * @see https://doc.babylonjs.com/extensions/WoodProceduralTexture
 */
struct WoodProceduralTextureScene : public IRenderableScene {

  WoodProceduralTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~WoodProceduralTextureScene() override = default;

  const char* getName() override
  {
    return "Wood Procedural Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2, 12.5f,
                                       Vector3::Zero(), scene);
    camera->attachControl(canvas, false);

    auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

    auto woodMaterial = StandardMaterial::New("WoodMaterial", scene);
    auto woodTexture
      = ProceduralTexturesLibrary::WoodProceduralTexture::New("WoodMaterialtext", 1024, scene);
    woodTexture->ampScale        = 80.f;
    woodMaterial->diffuseTexture = woodTexture;

    auto plane      = Mesh::CreatePlane("WoodPlane", 10, scene);
    plane->material = woodMaterial;
    plane->rotate(Vector3(1.f, 1.f, 0.5f), Math::PI / 3.f, Space::LOCAL);
  }

}; // end of struct WoodProceduralTextureScene

BABYLON_REGISTER_SAMPLE("Procedural Textures Library", WoodProceduralTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
