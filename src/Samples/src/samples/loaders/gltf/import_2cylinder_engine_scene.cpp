#include <babylon/samples/loaders/gltf/import_2cylinder_engine_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

Import2CylinderEngineScene::Import2CylinderEngineScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

Import2CylinderEngineScene::~Import2CylinderEngineScene()
{
}

const char* Import2CylinderEngineScene::getName()
{
  return "Import 2 Cylinder Engine Scene (glTF)";
}

void Import2CylinderEngineScene::initializeScene(ICanvas* /*canvas*/,
                                                 Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/2CylinderEngine/glTF/", "2CylinderEngine.gltf",
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
        camera->setTarget(Vector3(-35.f, 80.f, 0.f));
        camera->alpha  = Math::PI / 1.5f;
        camera->beta   = Math::PI / 3.5f;
        camera->radius = 250.f;
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
