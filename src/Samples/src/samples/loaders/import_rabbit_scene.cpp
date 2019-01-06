#include <babylon/samples/loaders/import_rabbit_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportRabbitScene::ImportRabbitScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ImportRabbitScene::~ImportRabbitScene()
{
}

const char* ImportRabbitScene::getName()
{
  return "Import Rabbit Scene";
}

void ImportRabbitScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "scenes/", "Rabbit.babylon", scene,
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
