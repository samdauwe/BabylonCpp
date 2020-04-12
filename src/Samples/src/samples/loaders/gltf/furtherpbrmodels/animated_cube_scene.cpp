#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Animated Cube Scene (glTF). A cube with non-smoothed faces, having a linear rotation
 * animation.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/AnimatedCube
 */
struct AnimatedCubeScene : public IRenderableScene {

  AnimatedCubeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~AnimatedCubeScene() override = default;

  const char* getName() override
  {
    return "Animated Cube Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/AnimatedCube/glTF/", "AnimatedCube.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3::Zero());
          camera->alpha  = 0.9f;
          camera->beta   = 1.f;
          camera->radius = Math::PI2 * 0.75f;
        }
      });
  }

}; // end of struct AnimatedCubeScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", AnimatedCubeScene)

} // end of namespace Samples
} // end of namespace BABYLON
