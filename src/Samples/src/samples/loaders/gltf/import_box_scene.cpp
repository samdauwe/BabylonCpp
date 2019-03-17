#include <babylon/samples/loaders/gltf/import_box_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportBoxScene::ImportBoxScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

ImportBoxScene::~ImportBoxScene()
{
}

const char* ImportBoxScene::getName()
{
  return "Import Box Scene (glTF)";
}

void ImportBoxScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/Box/glTF/", "Box.gltf", scene,
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
