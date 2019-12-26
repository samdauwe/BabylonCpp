#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Two Cylinder Engine Scene (glTF). Small CAD data set, including hierarchy.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/2CylinderEngine
 */
struct TwoCylinderEngineScene : public IRenderableScene {

  TwoCylinderEngineScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~TwoCylinderEngineScene() override = default;

  const char* getName() override
  {
    return "2 Cylinder Engine Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/2CylinderEngine/glTF/", "2CylinderEngine.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3(-35.f, 80.f, 0.f));
          camera->alpha  = Math::PI / 1.5f;
          camera->beta   = Math::PI / 3.5f;
          camera->radius = 250.f;
        }
      });
  }

}; // end of struct TwoCylinderEngineScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", TwoCylinderEngineScene)

} // end of namespace Samples
} // end of namespace BABYLON
