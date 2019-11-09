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
 * @brief Scene demonstrating the use of the simple material from the materials
 * library.
 */
struct SimpleMaterialScene : public IRenderableScene {

  SimpleMaterialScene(ICanvas* iCanvas);
  ~SimpleMaterialScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

}; // end of struct SimpleMaterialScene

} // end of namespace Samples
} // end of namespace BABYLON

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Material Scene (glTF).
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/cx20/gltf-test/tree/master/tutorialModels/SimpleMaterial
 */
struct SimpleMaterialGLTFScene : public IRenderableScene {

  SimpleMaterialGLTFScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~SimpleMaterialGLTFScene() override = default;

  const char* getName() override
  {
    return "Simple Material Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/SimpleMaterial/glTF/", "SimpleMaterial.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3(-0.5f, 0.5f, 0.f));
          camera->alpha  = Math::PI_2;
          camera->beta   = Math::PI_2;
          camera->radius = 2.121f;
        }
      });
  }

}; // end of struct SimpleMaterialGLTFScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", SimpleMaterialGLTFScene)

} // end of namespace Samples
} // end of namespace BABYLON
