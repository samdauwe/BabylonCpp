#include <babylon/animations/easing/sine_ease.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/guid.h>
#include <babylon/postprocesses/motion_blur_post_process.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Motion blur post-process Scene. Example demonstrating how to use the motion blur
 * post-process.
 * @see https://www.babylonjs-playground.com/#E5YGEL#2
 * @see https://doc.babylonjs.com/how_to/using_motionblurpostprocess
 */
class MotionBlurPostProcessScene : public IRenderableScene {

public:
  MotionBlurPostProcessScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~MotionBlurPostProcessScene() override = default;

  const char* getName() override
  {
    return "Motion blur post-process Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", -Math::PI_4, Math::PI / 2.5f, 200.f, Vector3::Zero(), scene);
    camera->attachControl(canvas, true);
    camera->minZ = 0.1f;
    camera->maxZ = 10000.f;

    // Environment Texture
    auto hdrTexture = CubeTexture::CreateFromPrefilteredData("textures/environment.dds", scene);

    scene->imageProcessingConfiguration()->exposure = 0.6f;
    scene->imageProcessingConfiguration()->contrast = 1.6f;

    // Skybox
    auto hdrSkybox                       = Mesh::CreateBox("hdrSkyBox", 1000.f, scene);
    auto hdrSkyboxMaterial               = PBRMaterial::New("skyBox", scene);
    hdrSkyboxMaterial->backFaceCulling   = false;
    hdrSkyboxMaterial->reflectionTexture = hdrTexture->clone();
    hdrSkyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    hdrSkyboxMaterial->microSurface                         = 1.f;
    hdrSkyboxMaterial->disableLighting                      = true;
    hdrSkybox->material                                     = hdrSkyboxMaterial;
    hdrSkybox->infiniteDistance                             = true;

    // Create material
    auto metal               = PBRMaterial::New("metal", scene);
    metal->reflectionTexture = hdrTexture;
    metal->microSurface      = 0.96f;
    metal->reflectivityColor = Color3(0.85f, 0.85f, 0.85f);
    metal->albedoColor       = Color3(0.01f, 0.01f, 0.01f);

    // Create spheres
    for (unsigned int i = 0; i < 50; i++) {
      auto x = Math::random() * (100.f - (-100.f)) - 100.f;
      auto y = Math::random() * (100.f - (-100.f)) - 100.f;
      auto z = Math::random() * (100.f - (-100.f)) - 100.f;

      auto ease = SineEase::New();
      ease->setEasingMode(EasingFunction::EASINGMODE_EASEINOUT);

      auto anim = Animation::New("animation", "position.y", 60, Animation::ANIMATIONTYPE_FLOAT(),
                                 Animation::ANIMATIONLOOPMODE_RELATIVE(), false);
      std::vector<IAnimationKey> animationKeys{
        IAnimationKey(0, AnimationValue(y)),
        IAnimationKey(30, AnimationValue(y + Math::random() * (250.f - (-250.f)) - 250.f)),
        IAnimationKey(60, AnimationValue(y)),
      };
      anim->setKeys(animationKeys);
      anim->setEasingFunction(ease);

      auto sphere = Mesh::CreateSphere("sphere", 48, 30.f, scene);
      sphere->id  = GUID::RandomId();
      sphere->position().set(x, y, z);
      sphere->material   = metal;
      sphere->animations = {anim};

      scene->beginAnimation(sphere, 0.f, 60.f, true, 1.f);
    }

    // Finally create the motion blur effect :)
    _mb = MotionBlurPostProcess::New("mb", scene, 1.f, camera);
  }

private:
  PostProcessPtr _mb;

}; // end of struct MotionBlurPostProcessScene

BABYLON_REGISTER_SAMPLE("Special FX", MotionBlurPostProcessScene)

} // end of namespace Samples
} // end of namespace BABYLON
