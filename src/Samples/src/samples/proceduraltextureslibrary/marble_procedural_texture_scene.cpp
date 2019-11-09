#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/proceduraltextureslibrary/marble/marble_procedural_texture.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief MarbleProceduralTextureScene Scene. Example demonstrating how to use marble procedural
 * texture.
 * @see https://www.babylonjs-playground.com/#HS1SK#4
 */
struct MarbleProceduralTextureScene : public IRenderableScene {

  MarbleProceduralTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~MarbleProceduralTextureScene() override = default;

  const char* getName() override
  {
    return "Marble Procedural Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2, 25.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);

    auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

    auto marbleMaterial = StandardMaterial::New("MarbleMaterial", scene);
    auto marbleTexture
      = ProceduralTexturesLibrary::MarbleProceduralTexture::New("MarbleMaterialtext", 512, scene);
    marbleTexture->numberOfTilesHeight = 3.f;
    marbleTexture->numberOfTilesWidth  = 3.f;
    marbleMaterial->diffuseTexture     = marbleTexture;

    auto plane      = Mesh::CreatePlane("MarblePlane", 20, scene);
    plane->material = marbleMaterial;
    plane->rotate(Vector3(1.f, 1.f, 0.5f), Math::PI / 3.f, Space::LOCAL);
  }

}; // end of struct MarbleProceduralTextureScene

BABYLON_REGISTER_SAMPLE("Procedural Textures Library", MarbleProceduralTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
