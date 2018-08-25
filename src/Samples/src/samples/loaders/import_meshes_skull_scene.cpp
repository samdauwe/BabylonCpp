#include <babylon/samples/loaders/import_meshes_skull_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Samples {

ImportMeshesSkullScene::ImportMeshesSkullScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _light{nullptr}, _camera{nullptr}
{
}

ImportMeshesSkullScene::~ImportMeshesSkullScene()
{
}

const char* ImportMeshesSkullScene::getName()
{
  return "Import Meshes Skull Scene";
}

void ImportMeshesSkullScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Adding a light
  _light = PointLight::New("Omni", Vector3(20.f, 20.f, 100.f), scene);

  // Adding an Arc Rotate Camera
  _camera
    = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);
  _camera->attachControl(canvas, true);

  // The first parameter can be used to specify which mesh to import. Here we
  // import all meshes
  SceneLoader::ImportMesh(
    {}, "scenes/", "skull.babylon", scene,
    [this](const vector_t<AbstractMeshPtr>& newMeshes,
           const vector_t<IParticleSystemPtr>& /*newParticleSystems*/,
           const vector_t<SkeletonPtr>& /*newSkeletons*/,
           const vector_t<AnimationGroupPtr>& /*newAnimationGroups*/) {
      // Set the target of the camera to the first imported mesh
      if (!newMeshes.empty()) {
        _camera->setTarget(newMeshes[0].get());
      }
    });

  // Move the light with the camera
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    _light->position = _camera->position;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
