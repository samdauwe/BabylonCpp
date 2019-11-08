#include <babylon/samples/samples_index.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/extensions/hexplanetgeneration/simple_planet.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/interfaces/irenderable_scene.h>


namespace BABYLON {

namespace Samples {

class ProceduralHexPlanetGenerationScene : public IRenderableScene {

public:
  ProceduralHexPlanetGenerationScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }

  ~ProceduralHexPlanetGenerationScene()
  {
  }

  const char* getName() override
  {
    return "Procedural Hex Planet Generation Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position
    auto camera = FreeCamera::New("camera1", Vector3(0, 0, -100.f), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a Sun & Moon
    auto sun        = HemisphericLight::New("sun", Vector3(0, 1.f, 0), scene);
    sun->intensity  = 0.96f;
    auto moon       = HemisphericLight::New("moon", Vector3(0, -1.f, 0), scene);
    moon->intensity = 0.25f;

    // Create the icosphere
    _hexPlanet = std::make_unique<Extensions::SimplePlanet>(2.f, 6.f, scene, "sdasy");
    _hexPlanet->initialize();
    _hexPlanet->render();

    _scene->registerBeforeRender([this](Scene*, EventState&) { _hexPlanet->revolve(); });
  }

private:
  std::unique_ptr<Extensions::SimplePlanet> _hexPlanet;

}; // end of class ProceduralHexPlanetGenerationScene


BABYLON_REGISTER_SAMPLE("Extensions", ProceduralHexPlanetGenerationScene)
} // end of namespace Samples
} // end of namespace BABYLON