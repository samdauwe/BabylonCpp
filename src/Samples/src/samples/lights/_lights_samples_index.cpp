#include <babylon/samples/lights/_lights_samples_index.h>

#include <babylon/samples/lights/lights_scene.h>

namespace BABYLON {
namespace Samples {

_LightsSamplesIndex::_LightsSamplesIndex()
{
  // -- Lights examples --

  // Lights Scene
  _samples["LightsScene"]
    = ::std::make_tuple(true,                                              //
                        [](ICanvas* iCanvas) {                             //
                          return ::std::make_unique<LightsScene>(iCanvas); //
                        });                                                //
}

_LightsSamplesIndex::~_LightsSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
