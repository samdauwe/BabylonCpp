#include <babylon/samples/loaders/import_candle_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportCandleScene::ImportCandleScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
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
