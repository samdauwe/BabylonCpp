#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/materialslibrary/water/water_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Float On Water Material Scene. Example demonstrating how to float on Water material.
 * @see https://www.babylonjs-playground.com/#L76FB1#49
 * @see https://doc.babylonjs.com/extensions/water
 */
struct FloatOnWaterMaterialScene : public IRenderableScene {

  FloatOnWaterMaterialScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~FloatOnWaterMaterialScene() override = default;

  const char* getName() override
  {
    return "Float On Water Material Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Camera
    auto camera = ArcRotateCamera::New("Camera", 3.f * Math::PI_2, Math::PI / 2.5f, 100.f,
                                       Vector3::Zero(), scene);
    camera->attachControl(canvas, true);

    // Light
    auto light       = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.98f;

    // Skybox
    auto skybox                       = Mesh::CreateBox("skyBox", 5000.f, scene);
    auto skyboxMaterial               = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling   = false;
    skyboxMaterial->reflectionTexture = CubeTexture::New("textures/skybox/TropicalSunnyDay", scene);
    skyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->diffuseColor                         = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->specularColor                        = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->disableLighting                      = true;
    skybox->material                                     = skyboxMaterial;

    // Water material
    auto waterMaterial
      = MaterialsLibrary::WaterMaterial::New("waterMaterial", scene, Vector2(512.f, 512.f));
    waterMaterial->bumpTexture      = Texture::New("textures/waterbump.png", scene);
    waterMaterial->windForce        = -10.f;
    waterMaterial->waveHeight       = 0.5f;
    waterMaterial->bumpHeight       = 0.1f;
    waterMaterial->waveLength       = 0.1f;
    waterMaterial->waveSpeed        = 50.f;
    waterMaterial->colorBlendFactor = 0.f;
    waterMaterial->windDirection    = Vector2(1.f, 1.f);
    waterMaterial->colorBlendFactor = 0.f;

    // Ground
    auto groundTexture    = Texture::New("textures/sand2.jpg", scene);
    groundTexture->vScale = groundTexture->uScale = 4.f;

    auto groundMaterial            = StandardMaterial::New("groundMaterial", scene);
    groundMaterial->diffuseTexture = groundTexture;

    auto ground          = Mesh::CreateGround("ground", 512, 512, 32, scene, false);
    ground->position().y = -1.f;
    ground->material     = groundMaterial;

    // Water mesh
    auto waterMesh      = Mesh::CreateGround("waterMesh", 512, 512, 32, scene, false);
    waterMesh->material = waterMaterial;

    // Sphere
    auto sphereMaterial            = StandardMaterial::New("sphereMaterial", scene);
    sphereMaterial->diffuseTexture = Texture::New("textures/wood.jpg", scene);

    auto sphere          = Mesh::CreateSphere("sphere", 32, 24, scene);
    sphere->position().y = 20.f;
    sphere->material     = sphereMaterial;

    // Configure water material
    waterMaterial->addToRenderList(ground);
    waterMaterial->addToRenderList(skybox);
    waterMaterial->addToRenderList(sphere);

    ////////// RAY CAST TO FIND WATER HEIGHT ////////////
    scene->registerBeforeRender(
      [waterMaterial, sphere](Scene* /*scene*/, EventState & /*es*/) -> void {
        auto time = waterMaterial->lastTime() / 100000.f;
        auto x    = sphere->position().x;
        auto z    = sphere->position().z;
        sphere->position().y
          = std::abs((std::sin(((x / 0.05f) + time * waterMaterial->waveSpeed))
                      * waterMaterial->waveHeight * waterMaterial->windDirection.x * 5.f)
                     + (std::cos(((z / 0.05f) + time * waterMaterial->waveSpeed))
                        * waterMaterial->waveHeight * waterMaterial->windDirection.y * 5.f));
      });
  }

}; // end of struct FloatOnWaterMaterialScene

BABYLON_REGISTER_SAMPLE("Materials Library", FloatOnWaterMaterialScene)

} // end of namespace Samples
} // end of namespace BABYLON
