#include <babylon/samples/loaders/gltf/import_triangle_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/lights/light.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportTriangleScene::ImportTriangleScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

ImportTriangleScene::~ImportTriangleScene()
{
}

const char* ImportTriangleScene::getName()
{
  return "Import Triangle Scene (glTF)";
}

void ImportTriangleScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/Triangle/glTF/", "Triangle.gltf", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      // Set the camera position
      auto camera
        = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera);
      if (camera) {
        camera->alpha  = Math::PI_2;
        camera->beta   = Math::PI_2;
        camera->radius = 2.121f;
        camera->setTarget(Vector3(-0.5f, 0.5f, 0.f));
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
