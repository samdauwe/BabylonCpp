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
 * @brief Simple Sparse Accessor Scene (glTF). A simple mesh that uses sparse accessors.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SimpleSparseAccessor
 */
struct SimpleSparseAccessorScene : public IRenderableScene {

  SimpleSparseAccessorScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~SimpleSparseAccessorScene() override = default;

  const char* getName() override
  {
    return "Simple Sparse Accessor Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/SimpleSparseAccessor/glTF/", "SimpleSparseAccessor.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3(-3.f, 2.f, 0.f));
          camera->alpha  = Math::PI_2;
          camera->beta   = Math::PI_2;
          camera->radius = Math::PI2;
        }
      });
  }

}; // end of struct SimpleSparseAccessorScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", SimpleSparseAccessorScene)

} // end of namespace Samples
} // end of namespace BABYLON
