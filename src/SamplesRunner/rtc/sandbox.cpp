#include <SamplesRunner/rtc/suppress_warnings.h>
#include <SamplesRunner/rtc/link_libraries.h>

#include "SamplesRunner/HelloScene.h"

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/mesh.h>

// This files demonstrates how to create a very simple renderable scene
struct HelloScene2 : public BABYLON::IRenderableScene
{
  HelloScene2() : IRenderableScene() {}
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
    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

    // Move the sphere upward 1/2 of its height
    sphere->position().y = 1.f;

    // Create a built-in "ground" shape.
    // Params: name, width, depth, subdivs, scene
    Mesh::CreateGround("ground1", 6, 6, 2, scene);
  }
};

std::shared_ptr<BABYLON::IRenderableScene> MakeHelloScene2()
{
  return std::make_shared<HelloScene2>();
}


#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>

#include <RuntimeObjectSystem/IObject.h>
#include <SamplesRunner/rtc/iscene_producer.h>
#include <SamplesRunner/rtc/interface_ids.h>
#include <iostream>

using namespace BABYLON::rtc;

class Sandbox : public TInterface<IID_RENDERABLESCENE_PRODUCER, ISceneProducer>
{
public:
  virtual void Update(float deltaTime)
  { 
    std::cout << "Runtime Object 01 update called!\n";
    std::cout << "Hey you, don't watch that\n";
    std::cout << "Watch this!\n";
  }
};

REGISTERCLASS(Sandbox);
