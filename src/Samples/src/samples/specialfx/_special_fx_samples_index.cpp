#include <babylon/samples/specialfx/_special_fx_samples_index.h>

#include <babylon/samples/specialfx/fireworks_with_shader_code_scene.h>
#include <babylon/samples/specialfx/fog_scene.h>
#include <babylon/samples/specialfx/infinite_loader_scene.h>
#include <babylon/samples/specialfx/waves_scene.h>

namespace BABYLON {
namespace Samples {

_SpecialFXSamplesIndex::_SpecialFXSamplesIndex()
{
  // -- Special FX examples --

  // Fireworks with Shader Code Scene
  _samples["FireworksWithShaderCodeScene"] = ::std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return ::std::make_unique<FireworksWithShaderCodeScene>(iCanvas); //
    });                                                                 //
  // Fog Scene
  _samples["FogScene"]
    = ::std::make_tuple(false,                                          //
                        [](ICanvas* iCanvas) {                          //
                          return ::std::make_unique<FogScene>(iCanvas); //
                        });                                             //
  // Infinite Loader Scene
  _samples["InfiniteLoaderScene"] = ::std::make_tuple(
    true,                                                      //
    [](ICanvas* iCanvas) {                                     //
      return ::std::make_unique<InfiniteLoaderScene>(iCanvas); //
    });                                                        //
  // Waves Scene
  _samples["WavesScene"]
    = ::std::make_tuple(true,                                             //
                        [](ICanvas* iCanvas) {                            //
                          return ::std::make_unique<WavesScene>(iCanvas); //
                        });                                               //
}

_SpecialFXSamplesIndex::~_SpecialFXSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
