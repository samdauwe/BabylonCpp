#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/loaders/babylon/_loaders_babylon_samples_index.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Candle Scene.
 * @see https://www.babylonjs-playground.com/#TSCJPP#1
 */
struct ImportCandleScene : public IRenderableScene {

  ImportCandleScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ImportCandleScene() override
  {
  }

  const char* getName() override
  {
    return "Import Candle Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "scenes/", "candle.babylon", scene,
                            [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
                                    const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                                    const std::vector<SkeletonPtr>& /*skeletons*/,
                                    const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              scene->createDefaultEnvironment();
                            });
  }

}; // end of struct ImportCandleScene

BABYLON_REGISTER_SAMPLE(_LoadersBabylonSamplesIndex::CategoryName(), ImportCandleScene)

} // end of namespace Samples
} // end of namespace BABYLON
