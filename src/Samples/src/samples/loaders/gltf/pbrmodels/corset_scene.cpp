#include <babylon/samples/loaders/gltf/pbrmodels/corset_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

CorsetScene::CorsetScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

CorsetScene::~CorsetScene()
{
}

const char* CorsetScene::getName()
{
  return "Corset Scene (glTF)";
}

void CorsetScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/Corset/glTF/", "Corset.gltf", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      // Set the camera position
      auto camera
        = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera);
      if (camera) {
        camera->setTarget(Vector3::Zero());
        camera->alpha  = Math::PI * (3.f / 4.f);
        camera->beta   = 1.f;
        camera->radius = Math::PI_4 / 5.f;
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
