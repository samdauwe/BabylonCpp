#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/proceduraltextureslibrary/grass/grass_procedural_texture.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief GrassProceduralTextureScene Scene. Example demonstrating how to use grass procedural
 * texture.
 * @see https://www.babylonjs-playground.com/#KM3TC#1
 * @see https://doc.babylonjs.com/extensions/GrassProceduralTexture
 */
struct GrassProceduralTextureScene : public IRenderableScene {

  GrassProceduralTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~GrassProceduralTextureScene() override = default;

  const char* getName() override
  {
    return "Grass Procedural Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", 2.5f * Math::PI_2, Math::PI_2, 12.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, false);

    auto light0 = HemisphericLight::New("Light", Vector3(0.f, 10.f, 0.f), scene);

    auto grassMaterial = StandardMaterial::New("GrassMaterial", scene);
    auto grassTexture
      = ProceduralTexturesLibrary::GrassProceduralTexture::New("GrassMaterialtext", 256, scene);
    grassMaterial->ambientTexture = grassTexture;

    auto plane      = Mesh::CreatePlane("GrassPlane", 10, scene);
    plane->material = grassMaterial;
    plane->rotate(Vector3(1.f, 1.f, 0.5f), Math::PI / 3.f, Space::LOCAL);
  }

}; // end of struct GrassProceduralTextureScene

BABYLON_REGISTER_SAMPLE("Procedural Textures Library", GrassProceduralTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
