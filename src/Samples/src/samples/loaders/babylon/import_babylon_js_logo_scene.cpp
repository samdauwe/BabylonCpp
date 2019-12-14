#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import BabylonJS Logo Scene.
 * @see https://www.babylonjs-playground.com/#17LHMG#1
 */
struct ImportBabylonJSLogoScene : public IRenderableScene {

  ImportBabylonJSLogoScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ImportBabylonJSLogoScene() override = default;

  const char* getName() override
  {
    return "Import Babylon JS Logo Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "scenes/", "babylonJS_logo_v3.babylon", scene,
                            [scene](const std::vector<AbstractMeshPtr>& /*newMeshes*/,
                                    const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
                                    const std::vector<SkeletonPtr>& /*newSkeletons*/,
                                    const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              scene->createDefaultEnvironment();
                            });
  }

}; // end of struct ImportBabylonJSLogoScene

BABYLON_REGISTER_SAMPLE("Loaders - babylon format", ImportBabylonJSLogoScene)

} // end of namespace Samples
} // end of namespace BABYLON
