#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Cube Scene (glTF). A cube with non-smoothed faces.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/Cube
 */
struct CubeScene : public IRenderableScene {

  CubeScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~CubeScene() override = default;

  const char* getName() override
  {
    return "Cube Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "glTF-Sample-Models/2.0/Cube/glTF/", "Cube.gltf", scene,
                            [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
                                    const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                                    const std::vector<SkeletonPtr>& /*skeletons*/,
                                    const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              // Set the camera position
                              auto camera
                                = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
                              if (camera) {
                                camera->setTarget(Vector3::Zero());
                                camera->alpha  = 0.9f;
                                camera->beta   = 1.f;
                                camera->radius = Math::PI2 * 0.75f;
                              }
                            });
  }

}; // end of struct CubeScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", CubeScene)

} // end of namespace Samples
} // end of namespace BABYLON
