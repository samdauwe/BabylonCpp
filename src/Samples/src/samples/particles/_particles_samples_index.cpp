#include <babylon/samples/particles/_particles_samples_index.h>

#include <babylon/samples/particles/triangles_sps_scene.h>

namespace BABYLON {
namespace Samples {

_ParticlesSamplesIndex::_ParticlesSamplesIndex()
{
  // -- Particles examples --

  // Triangles SPS Scene
  _samples["TrianglesSPSScene"]
    = std::make_tuple(false,                                                 //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<TrianglesSPSScene>(iCanvas); //
                      });                                                    //
}

_ParticlesSamplesIndex::~_ParticlesSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
