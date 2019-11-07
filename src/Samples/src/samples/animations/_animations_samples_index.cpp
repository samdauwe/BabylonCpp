#include <babylon/samples/animations/_animations_samples_index.h>

#include <babylon/samples/animations/easing/easing_functions_scene.h>
#include <babylon/samples/animations/easing/shape_easings_scene.h>
#include <babylon/samples/animations/pick_and_play_animation.h>
#include <babylon/samples/animations/pump_jack_scene.h>
#include <babylon/samples/animations/tube_animation_scene.h>

namespace BABYLON {
namespace Samples {


_AnimationsSamplesIndex::_AnimationsSamplesIndex()
{
  // -- Animations examples --

  // Pick And Play Animation Scene
  _samples["PickAndPlayAnimationScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PickAndPlayAnimationScene>(iCanvas);
  };
  // Pump Jack Scene
  _samples["PumpJackScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<PumpJackScene>(iCanvas); };
  // Tube Animation Scene
  _samples["TubeAnimationScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<TubeAnimationScene>(iCanvas);
  };

  // -- Easing function examples --

  // Easing Functions Scene
  _samples["EasingFunctionsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<EasingFunctionsScene>(iCanvas);
  };
  // Shape Easings Scene
  _samples["ShapeEasingsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<ShapeEasingsScene>(iCanvas);
  };
}

} // end of namespace Samples
} // end of namespace BABYLON