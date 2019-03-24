#include <babylon/samples/loaders/gltf/import_duck_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportDuckScene::ImportDuckScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

ImportDuckScene::~ImportDuckScene()
{
}

const char* ImportDuckScene::getName()
{
  return "Import Duck Scene (glTF)";
}

void ImportDuckScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/Duck/glTF/", "Duck.gltf", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      // Set the camera position
      auto camera
        = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera);
      if (camera) {
        camera->setTarget(Vector3(-35.f, 80.f, 0.f));
        camera->alpha  = Math::PI / 1.5f;
        camera->beta   = Math::PI / 3.5f;
        camera->radius = 250.f;
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
