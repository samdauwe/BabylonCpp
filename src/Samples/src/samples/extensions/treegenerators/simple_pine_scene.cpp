#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/extensions/treegenerators/simple_pine_generator.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/directional_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

struct SimplePineScene : public IRenderableScene {

  SimplePineScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~SimplePineScene() override = default;

  const char* getName() override
  {
    return "Simple Pine Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera = ArcRotateCamera::New("Camera", 1.f, 1.2f, 100.f, Vector3(0, 5, 0), scene);
    camera->setPosition(Vector3(0, -10, 130));

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
    auto pine = Extensions::SimplePineGenerator::CreateTree(scene, trunkMaterial, leafMaterial);
    pine->position().y -= 20.f;
  }

}; // end of struct SimplePineScene

BABYLON_REGISTER_SAMPLE("Extensions", SimplePineScene)

} // end of namespace Samples
} // end of namespace BABYLON
