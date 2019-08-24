#include <babylon/samples/loaders/gltf/pbrmodels/flight_helmet_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

FlightHelmetScene::FlightHelmetScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

FlightHelmetScene::~FlightHelmetScene()
{
}

const char* FlightHelmetScene::getName()
{
  return "Flight Helmet Scene (glTF)";
}

void FlightHelmetScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/FlightHelmet/glTF/", "FlightHelmet.gltf", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      // Set the camera position
      auto camera
        = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
      if (camera) {
        camera->setTarget(Vector3::Zero());
        camera->alpha  = Math::PI * (3.f / 4.f);
        camera->beta   = 1.f;
        camera->radius = Math::PI * 0.5f;
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
