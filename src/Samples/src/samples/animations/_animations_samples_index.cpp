#include <babylon/samples/animations/_animations_samples_index.h>

#include <babylon/samples/animations/animations_scene.h>
#include <babylon/samples/animations/cartoon_animations_scene.h>
#include <babylon/samples/animations/easing/easing_functions_scene.h>
#include <babylon/samples/animations/easing/shape_easings_scene.h>
#include <babylon/samples/animations/pump_jack_scene.h>
#include <babylon/samples/animations/tube_animation_scene.h>

namespace BABYLON {
namespace Samples {

_AnimationsSamplesIndex::_AnimationsSamplesIndex()
{
  // -- Animations examples --

  // Animations Scene
  _samples["AnimationsScene"] = ::std::make_tuple(
    true,                                                  //
    [](ICanvas* iCanvas) {                                 //
      return ::std::make_unique<AnimationsScene>(iCanvas); //
    });                                                    //
  // Cartoon Animations Scene
  _samples["CartoonAnimationsScene"] = ::std::make_tuple(
    false,                                                        //
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<CartoonAnimationsScene>(iCanvas); //
    });                                                           //
  // Pump Jack Scene
  _samples["PumpJackScene"]
    = ::std::make_tuple(false,                                               //
                        [](ICanvas* iCanvas) {                               //
                          return ::std::make_unique<PumpJackScene>(iCanvas); //
                        });                                                  //
  // Tube Animation Scene
  _samples["TubeAnimationScene"] = ::std::make_tuple(
    false,                                                    //
    [](ICanvas* iCanvas) {                                    //
      return ::std::make_unique<TubeAnimationScene>(iCanvas); //
    });                                                       //

  // -- Easing function examples --

  // Easing Functions Scene
  _samples["EasingFunctionsScene"] = ::std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<EasingFunctionsScene>(iCanvas); //
    });                                                         //
  // Shape Easings Scene
  _samples["ShapeEasingsScene"] = ::std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return ::std::make_unique<ShapeEasingsScene>(iCanvas); //
    });                                                      //
}

_AnimationsSamplesIndex::~_AnimationsSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
