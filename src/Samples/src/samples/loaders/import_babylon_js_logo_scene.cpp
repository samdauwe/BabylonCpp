#include <babylon/samples/loaders/import_babylon_js_logo_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportBabylonJSLogoScene::ImportBabylonJSLogoScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ImportBabylonJSLogoScene::~ImportBabylonJSLogoScene()
{
}

const char* ImportBabylonJSLogoScene::getName()
{
  return "Import Babylon JS Logo Scene";
}

void ImportBabylonJSLogoScene::initializeScene(ICanvas* /*canvas*/,
                                               Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "scenes/", "babylonJS_logo_v3.babylon", scene,
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
