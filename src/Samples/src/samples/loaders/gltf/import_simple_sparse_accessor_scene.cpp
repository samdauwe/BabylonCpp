#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/lights/light.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/samples/loaders/gltf/import_simple_sparse_accessor_scene.h>

namespace BABYLON {
namespace Samples {

ImportSimpleSparseAccessorScene::ImportSimpleSparseAccessorScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

ImportSimpleSparseAccessorScene::~ImportSimpleSparseAccessorScene()
{
}

const char* ImportSimpleSparseAccessorScene::getName()
{
  return "Import Simple Sparse Accessor Scene (glTF)";
}

void ImportSimpleSparseAccessorScene::initializeScene(ICanvas* /*canvas*/,
                                                      Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/SimpleSparseAccessor/glTF/",
    "SimpleSparseAccessor.gltf", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      // Set the camera position
      auto camera
        = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera);
      if (camera) {
        camera->setTarget(Vector3(-3.f, 2.f, 0.f));
        camera->alpha  = Math::PI_2;
        camera->beta   = Math::PI_2;
        camera->radius = Math::PI2;
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
