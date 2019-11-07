#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/loaders/babylon/_loaders_babylon_samples_index.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Rabbit Scene.
 * @see https://www.babylonjs-playground.com/#NMU4ZM#1
 */
struct ImportRabbitScene : public IRenderableScene {

  ImportRabbitScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ImportRabbitScene() override
  {
  }

  const char* getName() override
  {
    return "Import Rabbit Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "scenes/Rabbit/", "Rabbit.babylon", scene,
                            [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
                                    const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                                    const std::vector<SkeletonPtr>& /*skeletons*/,
                                    const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              scene->createDefaultEnvironment();
                            });
  }

}; // end of struct ImportRabbitScene

BABYLON_REGISTER_SAMPLE(_LoadersBabylonSamplesIndex::CategoryName(), ImportRabbitScene)

} // end of namespace Samples
} // end of namespace BABYLON
