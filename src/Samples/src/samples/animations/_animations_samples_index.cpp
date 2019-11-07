#include <babylon/samples/animations/_animations_samples_index.h>

#include <babylon/samples/animations/easing/easing_functions_scene.h>
#include <babylon/samples/animations/easing/shape_easings_scene.h>

namespace BABYLON {
namespace Samples {


_AnimationsSamplesIndex::_AnimationsSamplesIndex()
{
  // -- Animations examples --

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