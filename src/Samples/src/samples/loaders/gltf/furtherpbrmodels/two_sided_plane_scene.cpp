#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Two Sided Plane Scene (glTF). A plane having the two sided material parameter enabled.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/blob/master/2.0/TwoSidedPlane
 */
struct TwoSidedPlaneScene : public IRenderableScene {

  TwoSidedPlaneScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~TwoSidedPlaneScene() override = default;

  const char* getName() override
  {
    return "Two Sided Plane Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/TwoSidedPlane/glTF/", "TwoSidedPlane.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3::Zero());
          camera->alpha  = 2.5f;
          camera->beta   = 1.f;
          camera->radius = Math::PI2 * 0.65f;
        }
        // Change the light direction
        auto light = std::static_pointer_cast<HemisphericLight>(scene->lights[0]);
        if (light) {
          light->direction = Vector3(0.f, -1.f, 0.f);
        }
      });
  }

}; // end of struct TwoSidedPlaneScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", TwoSidedPlaneScene)

} // end of namespace Samples
} // end of namespace BABYLON
