#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Level of detail Scene. Example demonstrating how to use various meshes based on distance
 * to optimize rendering speed.
 * @see https://www.babylonjs-playground.com/#7HMHAU#1
 * @see https://doc.babylonjs.com/how_to/how_to_use_lod
 */
struct LevelOfDetailScene : public IRenderableScene {

  LevelOfDetailScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~LevelOfDetailScene() override = default;

  const char* getName() override
  {
    return "Level of detail Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = FreeCamera::New("Camera", Vector3(0.f, 0.f, 0.f), scene);
    camera->attachControl(canvas, true);

    auto hemi       = HemisphericLight::New("hemi", Vector3(0.f, 1.f, 0.f), scene);
    hemi->intensity = 0.97f;

    scene->fogColor = scene->clearColor;
    scene->fogMode  = Scene::FOGMODE_LINEAR;
    scene->fogStart = 10.f;
    scene->fogEnd   = 50.f;

    // Materials
    auto materialAmiga            = StandardMaterial::New("amiga", scene);
    auto diffuseTexture           = Texture::New("/textures/amiga.jpg", scene);
    materialAmiga->emissiveColor  = Color3(0.5f, 0.5f, 0.5f);
    diffuseTexture->uScale        = 5.f;
    diffuseTexture->vScale        = 5.f;
    materialAmiga->diffuseTexture = diffuseTexture;

    auto materialRed           = StandardMaterial::New("red", scene);
    materialRed->emissiveColor = Color3(0.5f, 0.f, 0.f);

    // Create a wall of knots
    auto count = 3;
    auto scale = 4.f;

    auto knot00 = Mesh::CreateTorusKnot("knot0", 0.5f, 0.2f, 128, 64, 2.f, 3.f, scene);
    auto knot01 = Mesh::CreateTorusKnot("knot1", 0.5f, 0.2f, 32, 16, 2.f, 3.f, scene);
    auto knot02 = Mesh::CreateTorusKnot("knot2", 0.5f, 0.2f, 24, 12, 2.f, 3.f, scene);
    auto knot03 = Mesh::CreateTorusKnot("knot3", 0.5f, 0.2f, 16, 8, 2.f, 3.f, scene);

    knot00->setEnabled(false);
    knot01->setEnabled(false);
    knot02->setEnabled(false);
    knot03->setEnabled(false);

    knot00->material = materialAmiga;
    knot01->material = materialAmiga;
    knot02->material = materialRed;
    knot03->material = materialRed;

    knot00->addLODLevel(15, knot01);
    knot00->addLODLevel(30, knot02);
    knot00->addLODLevel(45, knot03);
    knot00->addLODLevel(55, nullptr);

    for (int x = -count; x <= count; x++) {
      for (int y = -count; y <= count; y++) {
        for (size_t z = 5; z < 10; z++) {
          auto knot      = knot00->createInstance("knotI");
          knot->position = Vector3(x * scale, y * scale, z * scale);
        }
      }
    }
  }

}; // end of struct LevelOfDetailScene

BABYLON_REGISTER_SAMPLE("Optimizations", LevelOfDetailScene)

} // end of namespace Samples
} // end of namespace BABYLON
