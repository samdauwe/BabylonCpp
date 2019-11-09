#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Import Meshes Skull Scene. Example demonstrating how to Load a mesh from a file and add it
 * to the scene.
 * @see https://www.babylonjs-playground.com/#UKNERM#0
 * @see https://doc.babylonjs.com/how_to/load_from_any_file_type
 */
class ImportMeshesSkullScene : public IRenderableScene {

public:
  ImportMeshesSkullScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _light{nullptr}, _camera{nullptr}
  {
  }

  ~ImportMeshesSkullScene() override = default;

  const char* getName() override
  {
    return "Import Meshes Skull Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Adding a light
    _light = PointLight::New("Omni", Vector3(20.f, 20.f, 100.f), scene);

    // Adding an Arc Rotate Camera
    _camera = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);
    _camera->attachControl(canvas, true);

    // The first parameter can be used to specify which mesh to import. Here we
    // import all meshes
    SceneLoader::ImportMesh({}, "scenes/", "skull.babylon", scene,
                            [this](const std::vector<AbstractMeshPtr>& newMeshes,
                                   const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
                                   const std::vector<SkeletonPtr>& /*newSkeletons*/,
                                   const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
                              // Set the target of the camera to the first imported mesh
                              if (!newMeshes.empty()) {
                                _camera->setTarget(newMeshes[0]);
                              }
                            });

    // Move the light with the camera
    scene->registerBeforeRender(
      [this](Scene* /*scene*/, EventState& /*es*/) { _light->position = _camera->position; });
  }

private:
  PointLightPtr _light;
  ArcRotateCameraPtr _camera;

}; // end of class ImportMeshesSkullScene

BABYLON_REGISTER_SAMPLE("Loaders - babylon format", ImportMeshesSkullScene)

} // end of namespace Samples
} // end of namespace BABYLON
