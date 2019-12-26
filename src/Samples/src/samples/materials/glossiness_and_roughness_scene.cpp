#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Glossiness and roughness scene. Example demonstrating glossiness and rouphness features of
 * standard material.
 * @see https://www.babylonjs-playground.com/#RNBKQ#8
 * @see https://doc.babylonjs.com/api/classes/babylon.standardmaterial
 */
struct GlossinessAndRoughnessScene : public IRenderableScene {

  GlossinessAndRoughnessScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~GlossinessAndRoughnessScene() override = default;

  const char* getName() override
  {
    return "Glossiness and roughness scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("camera1", 0.f, Math::PI_2, 5.f, Vector3(0.f, 0.f, 0.f), scene);
    camera->attachControl(canvas, true);

    auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    auto sphere = Mesh::CreateSphere("sphere1", 16, 2, scene);

    auto mat                               = StandardMaterial::New("toto", scene);
    mat->specularTexture                   = Texture::New("textures/specularglossymap.png", scene);
    mat->specularPower                     = 64.f;
    mat->useGlossinessFromSpecularMapAlpha = true;
    mat->diffuseColor                      = Color3::Black();
    mat->roughness                         = 3.f;

    mat->reflectionTexture = CubeTexture::New("textures/skybox/skybox", scene);

    sphere->material = mat;

    // Skybox
    auto skybox                       = Mesh::CreateBox("skyBox", 100.f, scene);
    auto skyboxMaterial               = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling   = false;
    skyboxMaterial->reflectionTexture = CubeTexture::New("textures/skybox/skybox", scene);
    skyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->diffuseColor                         = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->specularColor                        = Color3(0.f, 0.f, 0.f);
    skybox->material                                     = skyboxMaterial;
  }

}; // end of struct GlossinessAndRoughnessScene

BABYLON_REGISTER_SAMPLE("Materials", GlossinessAndRoughnessScene)

} // end of namespace Samples
} // end of namespace BABYLON
