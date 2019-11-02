#include <babylon/samples/particles/_particles_samples_index.h>

#include <babylon/samples/particles/buildings_sps_scene.h>
#include <babylon/samples/particles/lighted_sps_scene.h>
#include <babylon/samples/particles/triangles_sps_scene.h>

namespace BABYLON {
namespace Samples {

_ParticlesSamplesIndex::_ParticlesSamplesIndex()
{
  // -- Particles examples --

  // Buildings SPS Scene
  _samples["BuildingsSPSScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<BuildingsSPSScene>(iCanvas);
  };
  // Lighted SPS Scene
  _samples["LightedSPSScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<LightedSPSScene>(iCanvas);
  };
  // Triangles SPS Scene
  _samples["TrianglesSPSScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<TrianglesSPSScene>(iCanvas);
  };
}

} // end of namespace Samples
} // end of namespace BABYLON
