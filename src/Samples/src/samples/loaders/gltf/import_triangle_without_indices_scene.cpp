#include <babylon/samples/loaders/gltf/import_triangle_without_indices_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/glTF/gltf_file_loader.h>
#include <babylon/loading/scene_loader.h>

namespace BABYLON {
namespace Samples {

TriangleWithoutIndicesScene::TriangleWithoutIndicesScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
  GLTF2::GLTFFileLoader::RegisterAsSceneLoaderPlugin();
}

TriangleWithoutIndicesScene::~TriangleWithoutIndicesScene()
{
}

const char* TriangleWithoutIndicesScene::getName()
{
  return "Import Triangle Without Indices Scene (glTF)";
}

void TriangleWithoutIndicesScene::initializeScene(ICanvas* /*canvas*/,
                                                  Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "glTF-Sample-Models/2.0/TriangleWithoutIndices/glTF/",
    "TriangleWithoutIndices.gltf", scene,
    [scene](const std::vector<AbstractMeshPtr>& /*meshes*/,
            const std::vector<IParticleSystemPtr>& /*particleSystems*/,
            const std::vector<SkeletonPtr>& /*skeletons*/,
            const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      scene->createDefaultEnvironment();
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
