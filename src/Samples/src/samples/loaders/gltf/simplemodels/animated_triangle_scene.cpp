#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/light.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Animated Triangle Scene (glTF). This sample is similar to the Triangle, but the node has a
 * rotation property that is modified with a simple animation.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Triangle
 */
struct AnimatedTriangleScene : public IRenderableScene {

  AnimatedTriangleScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~AnimatedTriangleScene() override = default;

  const char* getName() override
  {
    return "Animated Triangle Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/AnimatedTriangle/glTF/", "AnimatedTriangle.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3(0.f, 0.0f, 0.f));
          camera->alpha  = Math::PI_2;
          camera->beta   = Math::PI_2;
          camera->radius = Math::PI;
        }
      });
  }

}; // end of struct AnimatedTriangleScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", AnimatedTriangleScene)

} // end of namespace Samples
} // end of namespace BABYLON
