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
 * @brief Box Textured Non Power Of Two Scene (glTF). Box with a non-power-of-2 (NPOT) texture. Not
 * all implementations support NPOT textures.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoxTexturedNonPowerOfTwo
 */
struct BoxTexturedNonPowerOfTwoScene : public IRenderableScene {

  BoxTexturedNonPowerOfTwoScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~BoxTexturedNonPowerOfTwoScene() override = default;

  const char* getName() override
  {
    return "Box Textured Non Power Of Two Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "glTF-Sample-Models/2.0/BoxTexturedNonPowerOfTwo/glTF/",
                            "BoxTexturedNonPowerOfTwo.gltf", scene,
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
                                camera->radius = 2.5f;
                              }
                            });
  }

}; // end of struct BoxTexturedNonPowerOfTwoScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", BoxTexturedNonPowerOfTwoScene)

} // end of namespace Samples
} // end of namespace BABYLON
