#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

class LinesMeshSpiralScene : public IRenderableScene {

public:
  LinesMeshSpiralScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~LinesMeshSpiralScene() override = default;

  const char* getName() override
  {
    return "Lines Mesh Spiral Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera = ArcRotateCamera::New("Camera", 0.f, Math::PI / 2.f, 12.f, Vector3::Zero(), scene);

    // Change the camera position
    camera->setPosition(Vector3(0.f, 0.f, -10.f));

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("Light", Vector3(0, 1, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.98f;

    // Create the lines mesh spiral
    unsigned int tess = 3600;
    float radius      = 30.f;
    unsigned int rdNb = 50;   // spiral round number
    float zShift      = 20.f; // z shift factor
    float pi2         = Math::PI * 2.f;
    std::vector<Vector3> p;
    for (unsigned int i = 0; i <= tess; ++i) {
      float ang = i * pi2 / tess * rdNb;
      p.emplace_back(Vector3(radius * std::cos(ang) * i / tess, radius * std::sin(ang) * i / tess,
                             i / tess * zShift));
    }

    _circle        = Mesh::CreateLines("Circle", p, scene);
    _circle->color = Color3(1.f, 1.f, 1.f);

    _scene->registerBeforeRender([this](Scene*, EventState&) { _circle->rotation().z -= 0.02f; });
  }

private:
  LinesMeshPtr _circle;

}; // end of class LinesMeshSpiralScene

BABYLON_REGISTER_SAMPLE("Meshes", LinesMeshSpiralScene)

} // end of namespace Samples
} // end of namespace BABYLON
