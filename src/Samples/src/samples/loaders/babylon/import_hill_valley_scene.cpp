#include <babylon/samples/loaders/babylon/import_hill_valley_scene.h>

#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/material.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Hill Valley Scene.
 * @see https://www.babylonjs.com/demos/hillvalley/
 */
struct ImportHillValleyScene : public IRenderableScene {

  ImportHillValleyScene(ICanvas* iCanvas);
  ~ImportHillValleyScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct ImportHillValleyScene

ImportHillValleyScene::ImportHillValleyScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ImportHillValleyScene::~ImportHillValleyScene()
{
}

const char* ImportHillValleyScene::getName()
{
  return "Import Hill Valley Scene";
}

void ImportHillValleyScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // This creates and positions a free camera (non-mesh)
  auto camera = FreeCamera::New("camera1", Vector3(0.f, 5.f, -10.f), scene);

  // This targets the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // This attaches the camera to the canvas
  camera->attachControl(canvas, true);

  SceneLoader::Append(
    "scenes/hillvalley/", "HillValley.babylon", scene, [&canvas](Scene* scene) {
      if (scene->activeCamera()) {
        scene->activeCamera()->attachControl(canvas);

        auto activeCamera
          = std::static_pointer_cast<FreeCamera>(scene->activeCamera());
        if (activeCamera) {
          activeCamera->keysUp().emplace_back(90);    // Z
          activeCamera->keysUp().emplace_back(87);    // W
          activeCamera->keysDown().emplace_back(83);  // S
          activeCamera->keysLeft().emplace_back(65);  // A
          activeCamera->keysLeft().emplace_back(81);  // Q
          activeCamera->keysRight().emplace_back(69); // E
          activeCamera->keysRight().emplace_back(68); // D
        }
      }
      scene->collisionsEnabled = false;
      scene->lightsEnabled     = false;
      for (const auto& material : scene->materials) {
        material->checkReadyOnEveryCall = false;
      }
    });
}

std::unique_ptr<IRenderableScene> MakeImportHillValleyScene(ICanvas* iCanvas)
{
  return std::make_unique<ImportHillValleyScene>(iCanvas);
}

} // end of namespace Samples
} // end of namespace BABYLON
