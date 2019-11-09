#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Displacement map (CPU) Scene. Example demonstrating how to update mesh
 * geometry using a displacement map (CPU).
 * @see https://www.babylonjs-playground.com/#04JDPF#0
 */
struct DisplacementMapCpuScene : public IRenderableScene {

  DisplacementMapCpuScene(ICanvas* iCanvas) : IRenderableScene(iCanvas) {}
  ~DisplacementMapCpuScene() = default;
  const char* getName() override { return "Displacement map (CPU) Scene"; }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    auto camera
      = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    auto light = HemisphericLight::New("Omni0", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.98f;
    auto material    = StandardMaterial::New("kosh", scene);
    auto sphere      = Mesh::CreateSphere("Sphere", 32, 3, scene, true);
  
    camera->setPosition(Vector3(-10, 10, 0));
    camera->attachControl(canvas, true);
  
    sphere->applyDisplacementMap("/textures/amiga.jpg", 0, 1.5f);
  
    // Sphere material
    material->diffuseTexture = Texture::New("/textures/amiga.jpg", scene);
    sphere->material         = material;
  
    // Skybox
    auto skybox                     = Mesh::CreateBox("skyBox", 100.0, scene);
    auto skyboxMaterial             = StandardMaterial::New("skyBox", scene);
    skyboxMaterial->backFaceCulling = false;
    skyboxMaterial->reflectionTexture
      = CubeTexture::New("/textures/skybox/skybox", scene);
    skyboxMaterial->reflectionTexture()->coordinatesMode
      = TextureConstants::SKYBOX_MODE;
    skyboxMaterial->diffuseColor    = Color3(0, 0, 0);
    skyboxMaterial->specularColor   = Color3(0, 0, 0);
    skyboxMaterial->disableLighting = true;
    skybox->material                = skyboxMaterial;
    skybox->infiniteDistance        = true;
  }

}; // end of struct DisplacementMapCPUScene

BABYLON_REGISTER_SAMPLE("Meshes", DisplacementMapCpuScene)
} // end of namespace Samples
} // end of namespace BABYLON