#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Dude Scene.
 * @see https://www.babylonjs-playground.com/#WLDCUC#2
 */
struct ImportDudeScene : public IRenderableScene {

  ImportDudeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ImportDudeScene() override = default;

  const char* getName() override
  {
    return "Import Dude Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "scenes/Dude/", "Dude.babylon", scene,
                            [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
                                    const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                                    const std::vector<SkeletonPtr>& /*skeletons*/,
                                    const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              scene->createDefaultEnvironment();
                            });
  }

}; // end of struct ImportDudeScene

BABYLON_REGISTER_SAMPLE("Loaders - babylon format", ImportDudeScene)

} // end of namespace Samples
} // end of namespace BABYLON
