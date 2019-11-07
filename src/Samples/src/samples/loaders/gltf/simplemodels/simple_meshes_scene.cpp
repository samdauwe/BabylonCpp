#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/light.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/loaders/gltf/_loaders_gtlf_samples_index.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Meshes Scene (glTF). This is an example showing how the same mesh may be appended
 * to several nodes, to be rendered multiple times. It also shows how additional vertex attributes
 * are defined in a mesh.primitive - namely, attributes for the vertex normals.
 * @note The additional vertex normal attribute in this example is not yet used by any technique.
 * This may cause a warning to be printed during the validation. The normal attribute will be used
 * in the AdvancedMaterial example.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/SimpleMeshes
 */
struct SimpleMeshesScene : public IRenderableScene {

  SimpleMeshesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~SimpleMeshesScene() override
  {
  }

  const char* getName() override
  {
    return "Simple Meshes Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/SimpleMeshes/glTF/", "SimpleMeshes.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3(-1.f, 0.5f, 0.f));
          camera->alpha  = Math::PI_2;
          camera->beta   = Math::PI_2;
          camera->radius = 2.121f;
        }
      });
  }

}; // end of struct SimpleMeshesScene

BABYLON_REGISTER_SAMPLE(_LoadersGLTFSamplesIndex::CategoryName(), SimpleMeshesScene)

} // end of namespace Samples
} // end of namespace BABYLON
