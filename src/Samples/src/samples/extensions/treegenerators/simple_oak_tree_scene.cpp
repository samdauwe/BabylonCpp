#include <babylon/samples/extensions/treegenerators/simple_oak_tree_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/extensions/treegenerators/quick_tree_generator.h>
#include <babylon/lights/directional_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

SimpleOakTreeScene::SimpleOakTreeScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

SimpleOakTreeScene::~SimpleOakTreeScene()
{
}

const char* SimpleOakTreeScene::getName()
{
  return "Simple Oak Tree Scene";
}

void SimpleOakTreeScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto camera
    = ArcRotateCamera::New("Camera", 1.f, 1.2f, 100.f, Vector3(0, 5, 0), scene);
  camera->setPosition(Vector3(0, 0, 50));

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Material declarations
  auto trunkMaterial = StandardMaterial::New("woodMaterial", scene);
  trunkMaterial->setDiffuseColor(Color3::FromInts(102, 51, 0));

  auto leafMaterial = StandardMaterial::New("leafMaterial", scene);
  leafMaterial->setDiffuseColor(Color3(0.5f, 1.f, 0.5f));

  // Create a light
  auto light
    = DirectionalLight::New("dir01", Vector3(-0.5f, -1.f, -0.5f), scene);
  light->diffuse  = Color3(1.f, 1.f, 1.f);
  light->specular = Color3(0.f, 0.f, 0.f);
  light->position = Vector3(20.f, 40.f, 20.f);

  // Create 5 strict and 5 non-strict trees
  auto oakTree = Extensions::QuickTreeGenerator::CreateTree(
    15, 10, 5, trunkMaterial, leafMaterial, scene);
  oakTree->position().y -= 5.f;
}

} // end of namespace Samples
} // end of namespace BABYLON
