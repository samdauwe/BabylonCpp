#include <vector>

#include <babylon/engines/scene.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Suzanne Scene.
 * @see
 * https://www.davrous.com/2013/06/17/tutorial-part-3-learning-how-to-write-a-3d-soft-engine-in-c-ts-or-js-loading-meshes-exported-from-blender/
 * @see http://david.blob.core.windows.net/softengine3d/part6webgl/index.html
 */
class ImportSuzanneScene : public IRenderableScene {

public:
  ImportSuzanneScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~ImportSuzanneScene() override = default;

  const char* getName() override
  {
    return "Import Suzanne Scene";
  }

  void initializeScene(ICanvas* /*canvas*/, Scene* scene) override
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

private:
  std::vector<AbstractMeshPtr> _meshes;

}; // end of class ImportSuzanneScene

BABYLON_REGISTER_SAMPLE("Loaders - babylon format", ImportSuzanneScene)

} // end of namespace Samples
} // end of namespace BABYLON
