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
 * @brief Box Vertex Colors Scene (glTF). Box with vertex colors applied.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxVertexColors
 */
struct BoxVertexColorsScene : public IRenderableScene {

  BoxVertexColorsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~BoxVertexColorsScene() override = default;

  const char* getName() override
  {
    return "Box Vertex Colors Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh(
      {}, "glTF-Sample-Models/2.0/BoxVertexColors/glTF/", "BoxVertexColors.gltf", scene,
      [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
              const std::vector<IParticleSystemPtr>& /*particleSystems*/,
              const std::vector<SkeletonPtr>& /*skeletons*/,
              const std::vector<AnimationGroupPtr>& /*animationGroups*/,
              const std::vector<TransformNodePtr>& /*transformNodes*/,
              const std::vector<GeometryPtr>& /*geometries*/,
              const std::vector<LightPtr>& /*lights*/) {
        scene->createDefaultCameraOrLight(true, true, true);
        // Set the camera position
        auto camera = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
        if (camera) {
          camera->setTarget(Vector3::Zero());
          camera->alpha  = 0.9f;
          camera->beta   = 1.f;
          camera->radius = 2.5f;
        }
      });
  }

}; // end of struct BoxVertexColorsScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", BoxVertexColorsScene)

} // end of namespace Samples
} // end of namespace BABYLON
