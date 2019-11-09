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
 * @brief Simple Texture Scene (glTF).
 * Note that the texture image also shows the texture coordinates:
 * * The upper left point of the texture image has the texture coordinates (0,0)
 * * The lower right point of the texture image has the texture coordinates
 *   (1,1)
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/cx20/gltf-test/tree/master/tutorialModels/SimpleTexture
 */
struct SimpleTextureScene : public IRenderableScene {

  SimpleTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~SimpleTextureScene() override = default;

  const char* getName() override
  {
    return "Simple Texture Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/SimpleTexture/glTF/", "SimpleTexture.gltf", scene,
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

}; // end of struct SimpleTextureScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", SimpleTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
