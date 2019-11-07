#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/loaders/babylon/_loaders_babylon_samples_index.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Dummy 3 Scene.
 * @see https://www.babylonjs-playground.com/#C38BUD#1
 */
struct ImportDummy3Scene : public IRenderableScene {

  ImportDummy3Scene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ImportDummy3Scene() override
  {
  }

  const char* getName() override
  {
    return "Import Dummy 3 Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "scenes/", "dummy3.babylon", scene,
                            [scene](const std::vector<AbstractMeshPtr>& /*newMeshes*/,
                                    const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
                                    const std::vector<SkeletonPtr>& /*newSkeletons*/,
                                    const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              scene->createDefaultEnvironment();
                            });
  }

}; // end of struct ImportDummy3Scene

BABYLON_REGISTER_SAMPLE(_LoadersBabylonSamplesIndex::CategoryName(), ImportDummy3Scene)

} // end of namespace Samples
} // end of namespace BABYLON
