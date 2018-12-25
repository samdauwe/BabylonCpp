#include <babylon/samples/specialfx/volumetric_light_scattering_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
//#include <babylon/engine/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
//#include <babylon/materials/textures/texture_constants.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/mesh/mesh.h>
#include <babylon/postprocess/volumetric_light_scattering_post_process.h>

namespace BABYLON {
namespace Samples {

VolumetricLightScatteringScene::VolumetricLightScatteringScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _camera{nullptr}
{
}

VolumetricLightScatteringScene::~VolumetricLightScatteringScene()
{
}

const char* VolumetricLightScatteringScene::getName()
{
  return "Volumetric Light Scattering Scene";
}

void VolumetricLightScatteringScene::initializeScene(ICanvas* canvas,
                                                     Scene* scene)
{
  // Adding a light
  auto light = PointLight::New("Omni", Vector3(20.f, 20.f, 100.f), scene);

  // Adding an Arc Rotate Camera
  _camera
    = ArcRotateCamera::New("Camera", -0.5f, 2.2f, 100, Vector3::Zero(), scene);
  _camera->attachControl(canvas, true);

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
        auto firstMesh     = newMeshes[0].get();
        auto mat           = StandardMaterial::New("skull", _scene.get());
        mat->emissiveColor = Color3(0.2f, 0.2f, 0.2f);

        _camera->setTarget(firstMesh);
        firstMesh->material = mat;
      }
    });

  // Create the "God Rays" effect (volumetric light scattering)
  auto godrays = VolumetricLightScatteringPostProcess::New(
    "godrays", 1.f, _camera, nullptr, 100,
    TextureConstants::BILINEAR_SAMPLINGMODE, _engine.get(), false);

  // By default it uses a billboard to render the sun, just apply the desired
  // texture position and scale
  auto godrayMaterial
    = std::dynamic_pointer_cast<StandardMaterial>(godrays->mesh->material());
  auto sunTexture      = Texture::New("textures/sun.png", scene, true, false,
                                 TextureConstants::BILINEAR_SAMPLINGMODE);
  sunTexture->hasAlpha = true;
  godrayMaterial->diffuseTexture = sunTexture;
  godrays->mesh->position        = Vector3(-150.f, 150.f, 150.f);
  godrays->mesh->scaling         = Vector3(350.f, 350.f, 350.f);

  light->position = godrays->mesh->position();
}

} // end of namespace Samples
} // end of namespace BABYLON
