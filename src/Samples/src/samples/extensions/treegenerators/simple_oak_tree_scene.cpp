#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/extensions/treegenerators/quick_tree_generator.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

struct SimpleOakTreeScene : public IRenderableScene {

  SimpleOakTreeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~SimpleOakTreeScene() override = default;

  const char* getName() override
  {
    return "Simple Oak Tree Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", 1.f, 1.2f, 100.f, Vector3(0, 5, 0), scene);
    camera->setPosition(Vector3(0, 0, 50));

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Material declarations
    auto trunkMaterial          = StandardMaterial::New("woodMaterial", scene);
    trunkMaterial->diffuseColor = Color3::FromInts(102, 51, 0);

    auto leafMaterial          = StandardMaterial::New("leafMaterial", scene);
    leafMaterial->diffuseColor = Color3(0.5f, 1.f, 0.5f);

    // Create a light
    auto light      = DirectionalLight::New("dir01", Vector3(-0.5f, -1.f, -0.5f), scene);
    light->diffuse  = Color3(1.f, 1.f, 1.f);
    light->specular = Color3(0.f, 0.f, 0.f);
    light->position = Vector3(20.f, 40.f, 20.f);

    // Create 5 strict and 5 non-strict trees
    auto oakTree
      = Extensions::QuickTreeGenerator::CreateTree(15, 10, 5, trunkMaterial, leafMaterial, scene);
    oakTree->position().y -= 5.f;
  }

}; // end of struct SimpleOakTreeScene

BABYLON_REGISTER_SAMPLE("Extensions", SimpleOakTreeScene)

} // end of namespace Samples
} // end of namespace BABYLON
