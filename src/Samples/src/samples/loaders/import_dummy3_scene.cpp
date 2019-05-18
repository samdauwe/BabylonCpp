#include <babylon/samples/loaders/import_dummy3_scene.h>

#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportDummy3Scene::ImportDummy3Scene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ImportDummy3Scene::~ImportDummy3Scene()
{
}

const char* ImportDummy3Scene::getName()
{
  return "Import Dummy 3 Scene";
}

void ImportDummy3Scene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "scenes/", "dummy3.babylon", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*newMeshes*/,
            const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
            const std::vector<SkeletonPtr>& /*newSkeletons*/,
            const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      scene->createDefaultEnvironment();
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
