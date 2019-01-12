#include <babylon/samples/loaders/import_dude_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportDudeScene::ImportDudeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

ImportDudeScene::~ImportDudeScene()
{
}

const char* ImportDudeScene::getName()
{
  return "Import Dude Scene";
}

void ImportDudeScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "scenes/Dude/", "Dude.babylon", scene,
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
