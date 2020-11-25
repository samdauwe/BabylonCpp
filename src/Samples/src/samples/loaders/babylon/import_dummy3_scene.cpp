#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

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

  ~ImportDummy3Scene() override = default;

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
                                    const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/,
                                    const std::vector<TransformNodePtr>& /*transformNodes*/,
                                    const std::vector<GeometryPtr>& /*geometries*/,
                                    const std::vector<LightPtr>& /*lights*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              scene->createDefaultEnvironment();
                            });
  }

}; // end of struct ImportDummy3Scene

BABYLON_REGISTER_SAMPLE("Loaders - babylon format", ImportDummy3Scene)

} // end of namespace Samples
} // end of namespace BABYLON
