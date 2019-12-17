#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/light.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Alpha Blend Mode Test Scene (glTF). Tests alpha modes and settings.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AlphaBlendModeTest
 */
struct AlphaBlendModeTestScene : public IRenderableScene {

  AlphaBlendModeTestScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~AlphaBlendModeTestScene() override = default;

  const char* getName() override
  {
    return "Alpha Blend Mode Test Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/AlphaBlendModeTest/glTF/", "AlphaBlendModeTest.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3::Zero());
          camera->alpha  = Math::PI_2;
          camera->beta   = Math::PI_2;
          camera->radius = Math::PI * 3.f;
        }
      });
  }

}; // end of struct AlphaBlendModeTestScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", AlphaBlendModeTestScene)

} // end of namespace Samples
} // end of namespace BABYLON
