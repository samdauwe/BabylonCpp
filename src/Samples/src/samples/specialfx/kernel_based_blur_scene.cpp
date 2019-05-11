#include <babylon/samples/specialfx/kernel_based_blur_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/postprocesses/blur_post_process.h>

namespace BABYLON {
namespace Samples {

KernelBasedBlurScene::KernelBasedBlurScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _light{nullptr}, _camera{nullptr}
{
}

KernelBasedBlurScene::~KernelBasedBlurScene()
{
}

const char* KernelBasedBlurScene::getName()
{
  return "Kernel Based Blur Scene";
}

void KernelBasedBlurScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Adding a light
  _light = PointLight::New("Omni", Vector3(20.f, 20.f, 100.f), scene);

  // Adding an Arc Rotate Camera
  _camera
    = ArcRotateCamera::New("Camera", 0.f, 0.8f, 100.f, Vector3::Zero(), scene);
  _camera->attachControl(canvas, false);

  // The first parameter can be used to specify which mesh to import. Here we
  // import all meshes
  SceneLoader::ImportMesh(
    {}, "scenes/", "skull.babylon", scene,
    [this](const std::vector<AbstractMeshPtr>& newMeshes,
           const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
           const std::vector<SkeletonPtr>& /*newSkeletons*/,
           const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
      // Set the target of the camera to the first imported mesh
      if (!newMeshes.empty()) {
        _camera->setTarget(newMeshes[0].get());
      }
    });

  // Move the light with the camera
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    _light->position = _camera->position;
  });

  auto kernel = 40.f;
  // postProcess0
  BlurPostProcess::New("Horizontal blur", Vector2(1.f, 0.f), kernel, 1.f,
                       _camera);
  // postProcess1
  BlurPostProcess::New("Vertical blur", Vector2(0.f, 1.f), kernel, 1.f,
                       _camera);
}

} // end of namespace Samples
} // end of namespace BABYLON
