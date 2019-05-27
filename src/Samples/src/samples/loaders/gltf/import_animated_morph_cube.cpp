#include <babylon/samples/loaders/gltf/import_animated_morph_cube.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/lights/light.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

ImportAnimatedMorphCubeScene::ImportAnimatedMorphCubeScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

ImportAnimatedMorphCubeScene::~ImportAnimatedMorphCubeScene()
{
}

const char* ImportAnimatedMorphCubeScene::getName()
{
  return "Import Animated Morph Cube (glTF)";
}

void ImportAnimatedMorphCubeScene::initializeScene(ICanvas* /*canvas*/,
                                                   Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/AnimatedMorphCube/glTF/",
    "AnimatedMorphCube.gltf", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      // Set the camera position
      auto camera
        = std::static_pointer_cast<ArcRotateCamera>(scene->activeCamera);
      if (camera) {
        camera->setTarget(Vector3(0.f, 0.0f, 0.f));
        camera->alpha  = Math::PI / 12.f;
        camera->beta   = Math::PI / 3.f;
        camera->radius = Math::PI2;
      }
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
