#include <babylon/samples/loaders/import_candle_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/scene_loader.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>

namespace BABYLON {
namespace Samples {

ImportCandleScene::ImportCandleScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _light{nullptr}, _camera{nullptr}
{
}

ImportCandleScene::~ImportCandleScene()
{
}

const char* ImportCandleScene::getName()
{
  return "Import Candle Scene";
}

void ImportCandleScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  // Adding a light
  /*_light = PointLight::New("Omni", Vector3(20.f, 20.f, 100.f), scene);

  // Adding an Arc Rotate Camera
  _camera
    = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);
  _camera->attachControl(canvas, true);*/

  SceneLoader::ImportMesh(
    {}, "scenes/", "candle.babylon", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      scene->createDefaultEnvironment();
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
