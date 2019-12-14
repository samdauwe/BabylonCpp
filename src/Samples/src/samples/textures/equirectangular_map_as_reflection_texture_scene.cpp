#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Equirectangular map as reflection texture scene. Example demonstrating how to use
 * equirectangular maps as a reflection texture.
 * @see https://www.babylonjs-playground.com/#23IQHK#2
 */
struct EquirectangularMapAsReflectionTextureScene : public IRenderableScene {

  EquirectangularMapAsReflectionTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~EquirectangularMapAsReflectionTextureScene() override = default;

  const char* getName() override
  {
    return "Equirectangular Map as Reflection Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // This creates and positions a free camera (non-mesh)
    auto camera = ArcRotateCamera::New("camera1", 0.f, Math::PI_4, 6.f, Vector3::Zero(), scene);

    // This targets the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // This attaches the camera to the canvas
    camera->attachControl(canvas, true);

    // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
    auto light      = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    auto knot       = Mesh::CreateTorusKnot("knot", 2, 0.5f, 128, 64, 2, 3, scene);
    auto knot2      = Mesh::CreateTorusKnot("knot", 2, 0.5f, 128, 64, 2, 3, scene);
    auto knot3      = Mesh::CreateTorusKnot("knot", 2, 0.5f, 128, 64, 2, 3, scene);
    knot2->position = Vector3(10.f, 1.f, 0.f);
    knot3->position = Vector3(-10.f, -1.f, 0.f);
    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.1f;

    // Our built-in 'sphere' shape. Params: name, subdivs, size, scene
    auto sphere      = Mesh::CreateSphere("sphere1", 16, 2, scene);
    sphere->position = Vector3::Zero();

    auto mat                           = StandardMaterial::New("mat", scene);
    sphere->material                   = mat;
    knot->material                     = mat;
    knot2->material                    = mat;
    knot3->material                    = mat;
    auto reflectionTexture             = Texture::New("textures/equirectangular.jpg", scene, true);
    reflectionTexture->coordinatesMode = TextureConstants::EQUIRECTANGULAR_MODE;
    mat->reflectionTexture             = reflectionTexture;

    // Skybox
    auto skybox                        = Mesh::CreateBox("skyBox", 100.0, scene);
    auto skyboxMaterial                = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling    = false;
    reflectionTexture                  = Texture::New("textures/equirectangular.jpg", scene, true);
    reflectionTexture->coordinatesMode = TextureConstants::FIXED_EQUIRECTANGULAR_MODE;
    skyboxMaterial->reflectionTexture  = reflectionTexture;
    skyboxMaterial->disableLighting    = true;
    skybox->material                   = skyboxMaterial;
  }

}; // end of struct EquirectangularMapAsReflectionTextureScene

BABYLON_REGISTER_SAMPLE("Textures", EquirectangularMapAsReflectionTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
