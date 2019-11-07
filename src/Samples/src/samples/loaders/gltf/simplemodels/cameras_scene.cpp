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
 * @brief Cameras scene demonstrating the types of cameras in babylonjs.
 * @see https://www.babylonjs-playground.com/#1A3M5C#0
 * @see https://doc.babylonjs.com/babylon101/cameras
 */
struct CamerasScene : public IRenderableScene {

  CamerasScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~CamerasScene() override
  {
  }

  const char* getName() override
  {
    return "Cameras Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "glTF-Sample-Models/2.0/Cameras/glTF/", "Cameras.gltf", scene,
                            [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
                                    const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                                    const std::vector<SkeletonPtr>& /*skeletons*/,
                                    const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
                              scene->createDefaultCameraOrLight(true, true, true);
                              // Set the camera position
                              auto camera
                                = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera());
                              if (camera) {
                                camera->setTarget(Vector3(-0.5f, 0.5f, 0.f));
                                camera->alpha  = Math::PI_2;
                                camera->beta   = Math::PI_2;
                                camera->radius = 2.121f;
                              }
                            });
  }

}; // end of struct CamerasScene

BABYLON_REGISTER_SAMPLE(_LoadersGLTFSamplesIndex::CategoryName(), CamerasScene)

} // end of namespace Samples
} // end of namespace BABYLON
