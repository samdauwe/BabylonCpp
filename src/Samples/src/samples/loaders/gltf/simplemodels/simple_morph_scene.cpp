#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/light.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Morph Scene (glTF). A triangle with a morph animation applied.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SimpleMorph
 */
struct SimpleMorphScene : public IRenderableScene {

  SimpleMorphScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~SimpleMorphScene() override = default;

  const char* getName() override
  {
    return "Simple Morph Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/SimpleMorph/glTF/", "SimpleMorph.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3(-0.5f, 1.0f, 0.f));
          camera->alpha  = Math::PI_2;
          camera->beta   = Math::PI_4;
          camera->radius = Math::PI2 * 0.65f;
        }
      });
  }

}; // end of struct SimpleMorphScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", SimpleMorphScene)

} // end of namespace Samples
} // end of namespace BABYLON
