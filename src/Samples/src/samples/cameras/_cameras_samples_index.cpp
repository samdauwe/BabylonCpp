#include <babylon/samples/cameras/_cameras_samples_index.h>

#include <babylon/samples/cameras/cameras_scene.h>

namespace BABYLON {
namespace Samples {

_CamerasSamplesIndex::_CamerasSamplesIndex()
{
  // -- Cameras examples --

  // Cameras Scene
  _samples["CamerasScene"]
    = ::std::make_tuple(true,                                               //
                        [](ICanvas* iCanvas) {                              //
                          return ::std::make_unique<CamerasScene>(iCanvas); //
                        });                                                 //
}

_CamerasSamplesIndex::~_CamerasSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
