#include <babylon/samples/extensions/_extensions_samples_index.h>

#include <babylon/samples/extensions/hexplanetgeneration/hex_ico_sphere_scene.h>
#include <babylon/samples/extensions/hexplanetgeneration/procedural_hex_planet_generation_scene.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_01.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_02.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_03.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_04.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_05.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_06.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_07.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_08.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_09.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_10.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_11.h>
#include <babylon/samples/extensions/noisegeneration/perlin_noise_waves_scene.h>
#include <babylon/samples/extensions/polyhedron/polyhedra_scene.h>
#include <babylon/samples/extensions/treegenerators/simple_oak_tree_scene.h>
#include <babylon/samples/extensions/treegenerators/simple_pine_scene.h>

namespace BABYLON {
namespace Samples {

_ExtensionsSamplesIndex::_ExtensionsSamplesIndex()
{
  // -- Hex planet generation examples --

  // Hex Icosphere Scene
  _samples["HexIcosphereScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<HexIcosphereScene>(iCanvas);
  };
  // Procedural Hex Planet Generation Scene
  _samples["ProceduralHexPlanetGenerationScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ProceduralHexPlanetGenerationScene>(iCanvas);
  };

  // -- Navigation examples--

  // Crowd Simulation Scene 01
  _samples["CrowdSimulationScene01"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene01>(iCanvas);
  };
  // Crowd Simulation Scene 02
  _samples["CrowdSimulationScene02"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene02>(iCanvas);
  };
  // Crowd Simulation Scene 03
  _samples["CrowdSimulationScene03"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene03>(iCanvas);
  };
  // Crowd Simulation Scene 04
  _samples["CrowdSimulationScene04"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene04>(iCanvas);
  };
  // Crowd Simulation Scene 05
  _samples["CrowdSimulationScene05"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene05>(iCanvas);
  };
  // Crowd Simulation Scene 06
  _samples["CrowdSimulationScene06"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene06>(iCanvas);
  };
  // Crowd Simulation Scene 07
  _samples["CrowdSimulationScene07"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene07>(iCanvas);
  };
  // Crowd Simulation Scene 08
  _samples["CrowdSimulationScene08"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene08>(iCanvas);
  };
  // Crowd Simulation Scene 09
  _samples["CrowdSimulationScene09"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene09>(iCanvas);
  };
  // Crowd Simulation Scene 10
  _samples["CrowdSimulationScene10"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene10>(iCanvas);
  };
  // Crowd Simulation Scene 11
  _samples["CrowdSimulationScene11"] = [](ICanvas* iCanvas) {
    return std::make_unique<CrowdSimulationScene11>(iCanvas);
  };

  // -- Noise generation examples --
  // Perlin Noise Waves Scene
  _samples["PerlinNoiseWavesScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PerlinNoiseWavesScene>(iCanvas);
  };

  // -- Polyhedron examples --

  // Polyhedra Scene
  _samples["PolyhedraScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PolyhedraScene>(iCanvas);
  };

  // -- Tree generators examples --

  // Simple Oak Tree Scene
  _samples["SimpleOakTreeScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleOakTreeScene>(iCanvas);
  };
  // Simple Pine Scene
  _samples["SimplePineScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimplePineScene>(iCanvas);
  };
}

} // end of namespace Samples
} // end of namespace BABYLON
