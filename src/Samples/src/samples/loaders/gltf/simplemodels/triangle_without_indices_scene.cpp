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
 * @brief Triangle Without Indices Scene (glTF). The simplest possible glTF asset: A single scene
 * with a single node and a single mesh with a single mesh.primitive with a single triangle with a
 * single attribute, without indices and without a material.
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 * @see https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/TriangleWithoutIndices
 */
struct TriangleWithoutIndicesScene : public IRenderableScene {

  TriangleWithoutIndicesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
    GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
  }

  ~TriangleWithoutIndicesScene() override = default;

  const char* getName() override
  {
    return "Triangle Without Indices Scene (glTF)";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
  {
    SceneLoader::ImportMesh({}, "glTF-Sample-Models/2.0/TriangleWithoutIndices/glTF/",
                            "TriangleWithoutIndices.gltf", scene,
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

}; // end of struct TriangleWithoutIndicesScene

BABYLON_REGISTER_SAMPLE("Loaders - glTF format", TriangleWithoutIndicesScene)

} // end of namespace Samples
} // end of namespace BABYLON
