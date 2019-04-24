#include <vector>
#include <babylon/samples/loaders/import_suzanne_scene.h>

#include <babylon/engine/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {
namespace Samples {

ImportSuzanneScene::ImportSuzanneScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ImportSuzanneScene::~ImportSuzanneScene()
{
}

const char* ImportSuzanneScene::getName()
{
  return "Import Suzanne Scene";
}

void ImportSuzanneScene::initializeScene(ICanvas* /*canvas*/, Scene* scene)
{
  SceneLoader::ImportMesh(
    {}, "scenes/Suzanne/", "Suzanne.babylon", scene,
    [this, scene](const std::vector<AbstractMeshPtr>& meshes,
                  const std::vector<IParticleSystemPtr>& /*particleSystems*/,
                  const std::vector<SkeletonPtr>& /*skeletons*/,
                  const std::vector<AnimationGroupPtr>& /*animationGroups*/) {
      scene->createDefaultCameraOrLight(true, true, true);
      scene->createDefaultEnvironment();
      // Animation
      _meshes = meshes;
      scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
        for (const auto& mesh : _meshes) {
          // rotating slightly the mesh during each frame rendered
          mesh->rotation().y += 0.01f;
        }
      });
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
