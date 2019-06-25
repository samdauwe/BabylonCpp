#include <babylon/samples/loaders/gltf/furtherpbrmodels/antique_camera_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

AntiqueCameraScene::AntiqueCameraScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

AntiqueCameraScene::~AntiqueCameraScene()
{
}

const char* AntiqueCameraScene::getName()
{
  return "Antique Camera Scene (glTF)";
}

void AntiqueCameraScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/AntiqueCamera/glTF/", "AntiqueCamera.gltf",
    scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      // Set the camera position
      auto camera
        = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera);
      if (camera) {
        camera->setTarget(Vector3(0.f, 3.5f, 0.f));
        camera->alpha  = 0.9f;
        camera->beta   = 1.f;
        camera->radius = Math::PI2 * 1.8f;
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
