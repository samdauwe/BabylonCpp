#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/fresnel_parameters.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Refraction and reflection dds texture scene. This example simulates how light would
 * reflect and refract with a sphere using a DDS texure as skybox.
 * @see https://www.babylonjs-playground.com/#XH85A9#0
 */
class RefractionAndReflectionDdsScene : public IRenderableScene {

public:
  RefractionAndReflectionDdsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~RefractionAndReflectionDdsScene() override = default;

  const char* getName() override
  {
    return "Refraction And Reflection DDS Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera   = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    auto material = StandardMaterial::New("kosh", scene);
    auto sphere1  = Mesh::CreateSphere("Sphere1", 32, 5, scene);
    auto light    = PointLight::New("Omni0", Vector3(-17.6f, 18.8f, -49.9f), scene);

    camera->setPosition(Vector3(-15.f, 3.f, 0.f));
    camera->attachControl(canvas, true);

    // Sphere1 material
    material->refractionTexture = CubeTexture::New("textures/SpecularHDR.dds", scene);
    material->reflectionTexture = CubeTexture::New("textures/SpecularHDR.dds", scene);
    material->diffuseColor      = Color3(0.f, 0.f, 0.f);
    material->invertRefractionY = false;
    material->indexOfRefraction = 0.98f;
    material->specularPower     = 128.f;
    sphere1->material           = material;

    material->refractionFresnelParameters()->power      = 2.f;
    material->reflectionFresnelParameters()->power      = 2.f;
    material->reflectionFresnelParameters()->leftColor  = Color3::Black();
    material->reflectionFresnelParameters()->rightColor = Color3::White();

    // Skybox
    auto skybox                       = Mesh::CreateBox("skyBox", 100.f, scene);
    auto skyboxMaterial               = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling   = false;
    skyboxMaterial->reflectionTexture = CubeTexture::New("textures/SpecularHDR.dds", scene);
    skyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->diffuseColor                         = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->specularColor                        = Color3(0.f, 0.f, 0.f);
    skyboxMaterial->disableLighting                      = true;
    skybox->material                                     = skyboxMaterial;
  }

}; // end of class RefractionAndReflectionDDSScene

BABYLON_REGISTER_SAMPLE("Special FX", RefractionAndReflectionDdsScene)

} // end of namespace Samples
} // end of namespace BABYLON
