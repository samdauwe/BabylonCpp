#include <babylon/samples/extensions/hexplanetgeneration/procedural_hex_planet_generation_scene.h>

#include <babylon/cameras/free_camera.h>
#include <babylon/core/logging.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/extensions/hexplanetgeneration/simple_planet.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data.h>

namespace BABYLON {
namespace Samples {

ProceduralHexPlanetGenerationScene::ProceduralHexPlanetGenerationScene(
  ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ProceduralHexPlanetGenerationScene::~ProceduralHexPlanetGenerationScene()
{
}

const char* ProceduralHexPlanetGenerationScene::getName()
{
  return "Procedural Hex Planet Generation Scene";
}

void ProceduralHexPlanetGenerationScene::initializeScene(ICanvas* canvas,
                                                         Scene* scene)
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
  _hexPlanet
    = std::make_unique<Extensions::SimplePlanet>(2.f, 6, scene, "sdasy");
  _hexPlanet->initialize();
  _hexPlanet->render();

  _scene->registerBeforeRender(
    [this](Scene*, EventState&) { _hexPlanet->revolve(); });
}

} // end of namespace Samples
} // end of namespace BABYLON
