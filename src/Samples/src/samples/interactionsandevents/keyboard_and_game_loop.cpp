#include <babylon/cameras/free_camera.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/mesh.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Keyboard + Game Loop Scene. Example demonstrating how to move a sphere around with the
 * keyboard.
 * @see https://www.babylonjs-playground.com/#15EY4F#0
 * @see https://doc.babylonjs.com/how_to/how_to_use_actions
 */
struct KeyboardAndGameLoopScene : public IRenderableScene {

  KeyboardAndGameLoopScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~KeyboardAndGameLoopScene() override = default;

  const char* getName() override
  {
    return "Keyboard + Game Loop Scene";
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

    // Create a built-in "sphere" shape; its constructor takes 5 params: name,
    // subdivs, size, scene
    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

    // Move the sphere upward 1/2 of its height
    sphere->position().y = 1.f;

    // Game/Render loop
    canvas->addKeyEventListener(EventType::KEY_DOWN, [sphere](KeyboardEvent&& evt) {
      if ((evt.keyCode == 90) || (evt.keyCode == 265)) { // "w" or "ArrowUp"
        sphere->position().z += 0.1f;
      }
      else if ((evt.keyCode == 81) || (evt.keyCode == 263)) { // "a" or "ArrowLeft"
        sphere->position().x -= 0.1f;
      }
      else if ((evt.keyCode == 83) || (evt.keyCode == 264)) { // "s" or "ArrowDown"
        sphere->position().z -= 0.1f;
      }
      else if ((evt.keyCode == 68) || (evt.keyCode == 262)) { // "d" or "ArrowRight"
        sphere->position().x += 0.1f;
      }
    });
  }

}; // end of struct KeyboardAndGameLoopScene

BABYLON_REGISTER_SAMPLE("Interactions & events", KeyboardAndGameLoopScene)

} // end of namespace Samples
} // end of namespace BABYLON
