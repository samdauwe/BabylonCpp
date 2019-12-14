#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Render Target Texture Scene. Example demonstrating how to render to a texture.
 * @see https://www.babylonjs-playground.com/#1ELSIY#0
 */
struct RenderTargetTextureScene : public IRenderableScene {

  RenderTargetTextureScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~RenderTargetTextureScene() override = default;

  const char* getName() override
  {
    return "Render Target Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position to (x:0, y:5, z:-10)
    auto camera = FreeCamera::New("camera1", Vector3(0, 5, -10), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.7f;

    // Create a built-in "sphere" shape; its constructor takes 4 params: name,
    // subdivs, size, scene
    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

    // Move the sphere upward 1/2 of its height
    sphere->position().y = 1.f;

    // Create a built-in "ground" shape.
    // Params: name, width, depth, subdivs, scene
    auto ground         = Mesh::CreateGround("ground1", 6, 6, 2, scene);
    auto groundMaterial = StandardMaterial::New("ground1Material", scene);
    ground->material    = groundMaterial;

    // Create the render target texture
    auto renderTexture = RenderTargetTexture::New("render", 512.f, scene);
    renderTexture->renderList().emplace_back(sphere.get());
    groundMaterial->diffuseTexture = renderTexture;

    scene->customRenderTargets.emplace_back(renderTexture.get());
  }

}; // end of struct RenderTargetTextureScene

BABYLON_REGISTER_SAMPLE("Textures", RenderTargetTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
