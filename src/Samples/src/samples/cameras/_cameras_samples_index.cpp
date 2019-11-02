#include <babylon/samples/cameras/_cameras_samples_index.h>

#include <babylon/samples/cameras/cameras_scene.h>
#include <babylon/samples/cameras/follow_camera_scene.h>
#include <babylon/samples/cameras/multi_views_scene.h>
#include <babylon/samples/cameras/rollercoaster_scene.h>

namespace BABYLON {
namespace Samples {

_CamerasSamplesIndex::_CamerasSamplesIndex()
{
  // -- Cameras examples --

  // Cameras Scene
  _samples["CamerasScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<CamerasScene>(iCanvas); };
  // Follow Camera Scene
  _samples["FollowCameraScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<FollowCameraScene>(iCanvas);
  };
  // Multi-Views Scene
  _samples["MultiViewsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<MultiViewsScene>(iCanvas);
  };
  // Rollercoaster Scene
  _samples["RollercoasterScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<RollercoasterScene>(iCanvas);
  };
}

} // end of namespace Samples
} // end of namespace BABYLON
