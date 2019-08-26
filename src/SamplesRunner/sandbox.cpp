#ifdef BABYLON_BUILD_SANDBOX

// This is the sandbox : you can here enter / modify code during 
// the execution, and the scene will be updated automatically!
// You do not need to exit the application!

// Do not remove the include below, 
// it is needed to enable "Runtime Compilation"
#include "SamplesRunner/rtc/sandbox_autocompile.h"

// You can edit the code below freely (add includes, modify the scene, etc)
#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/mesh.h>

struct SandboxScene : public BABYLON::IRenderableScene
{
  SandboxScene() : IRenderableScene() {}
  const char* getName() override { return "Hello Scene"; }

  void initializeScene(BABYLON::ICanvas* canvas, BABYLON::Scene* scene) override
  {
    using namespace BABYLON;
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

    // Create a built-in "sphere" shape; its constructor takes 5 params: name,
    // subdivs, size, scene
    auto sphere = Mesh::CreateSphere("sphere1", 16, 1.5f, scene);

    // Move the sphere upward 1/2 of its height
    sphere->position().y = 1.f;

    // Create a built-in "ground" shape.
    // Params: name, width, depth, subdivs, scene
    Mesh::CreateGround("ground1", 6, 6, 2, scene);
  }
};








////////////////////////////////////////////
// Do not edit below, these lines enable the Runtime Compilation
REGISTERCLASS(Sandbox);
std::shared_ptr<BABYLON::IRenderableScene> Sandbox::MakeScene() {
  return std::make_shared<SandboxScene>();
}

#endif // #ifdef BABYLON_BUILD_SANDBOX





