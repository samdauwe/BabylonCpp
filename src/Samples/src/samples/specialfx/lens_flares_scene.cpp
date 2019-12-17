#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lensflares/lens_flare.h>
#include <babylon/lensflares/lens_flare_system.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Lens Flares Scene. This example demonstrates how to simulate lens flares on the camera.
 * @see https://www.babylonjs-playground.com/#ZEB7H6#0
 */
struct LensFlaresScene : public IRenderableScene {

  LensFlaresScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~LensFlaresScene() override = default;

  const char* getName() override
  {
    return "Lens Flares Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera       = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    auto light0       = PointLight::New("Omni0", Vector3(21.84f, 50.f, -28.26f), scene);
    light0->intensity = 0.98f;

    camera->alpha = 2.8f;
    camera->beta  = 2.25f;

    camera->attachControl(canvas, true);

    // Creating light sphere
    auto lightSphere0 = Mesh::CreateSphere("Sphere0", 16, 0.5f, scene);

    auto mat               = StandardMaterial::New("white", scene);
    mat->diffuseColor      = Color3(0, 0, 0);
    mat->specularColor     = Color3(0, 0, 0);
    mat->emissiveColor     = Color3(1, 1, 1);
    lightSphere0->material = mat;

    lightSphere0->position = light0->position();

    auto lensFlareSystem = LensFlareSystem::New("lensFlareSystem", light0, scene);
    // flare00
    LensFlare::New(0.2f, 0.f, Color3(1.f, 1.f, 1.f), "/textures/flare2.png", lensFlareSystem);
    // flare01
    LensFlare::New(0.5f, 0.2f, Color3(0.5f, 0.5f, 1.f), "/textures/flare3.png", lensFlareSystem);
    // flare02
    LensFlare::New(0.2f, 1.f, Color3(1.f, 1.f, 1.f), "/textures/flare3.png", lensFlareSystem);
    // flare03
    LensFlare::New(0.4f, 0.4f, Color3(1.f, 0.5f, 1.f), "/textures/flare.png", lensFlareSystem);
    // flare04
    LensFlare::New(0.1f, 0.6f, Color3(1.f, 1.f, 1.f), "/textures/flare2.png", lensFlareSystem);
    // flare05
    LensFlare::New(0.3f, 0.8f, Color3(1.f, 1.f, 1.f), "/textures/flare3.png", lensFlareSystem);

    // Skybox
    auto skybox                       = Mesh::CreateBox("skyBox", 100.f, scene);
    auto skyboxMaterial               = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling   = false;
    skyboxMaterial->reflectionTexture = CubeTexture::New("textures/skybox/skybox", scene);
    skyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->diffuseColor                         = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->specularColor                        = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->disableLighting                      = true;
    skybox->material                                     = skyboxMaterial;
  }

}; // end of struct LensFlaresScene

BABYLON_REGISTER_SAMPLE("Special FX", LensFlaresScene)

} // end of namespace Samples
} // end of namespace BABYLON
