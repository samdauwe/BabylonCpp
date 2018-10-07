#include <babylon/samples/specialfx/_special_fx_samples_index.h>

#include <babylon/samples/specialfx/environment_scene.h>
#include <babylon/samples/specialfx/fireworks_with_shader_code_scene.h>
#include <babylon/samples/specialfx/fog_scene.h>
#include <babylon/samples/specialfx/fresnel_scene.h>
#include <babylon/samples/specialfx/infinite_loader_scene.h>
#include <babylon/samples/specialfx/sprites_scene.h>
#include <babylon/samples/specialfx/waves_scene.h>

namespace BABYLON {
namespace Samples {

_SpecialFXSamplesIndex::_SpecialFXSamplesIndex()
{
  // -- Special FX examples --

  // Environment Scene
  _samples["EnvironmentScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<EnvironmentScene>(iCanvas); //
                      });                                                   //
  // Fireworks with Shader Code Scene
  _samples["FireworksWithShaderCodeScene"] = std::make_tuple(
    true,                                                             //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<FireworksWithShaderCodeScene>(iCanvas); //
    });                                                               //
  // Fog Scene
  _samples["FogScene"]
    = std::make_tuple(true,                                         //
                      [](ICanvas* iCanvas) {                        //
                        return std::make_unique<FogScene>(iCanvas); //
                      });                                           //
  // Fresnel Scene
  _samples["FresnelScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<FresnelScene>(iCanvas); //
                      });                                               //
  // Infinite Loader Scene
  _samples["InfiniteLoaderScene"] = std::make_tuple(
    false,                                                   //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<InfiniteLoaderScene>(iCanvas); //
    });                                                      //
  // Sprites Scene
  _samples["SpritesScene"]
    = std::make_tuple(false,                                            //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<SpritesScene>(iCanvas); //
                      });                                               //
  // Waves Scene
  _samples["WavesScene"]
    = std::make_tuple(true,                                           //
                      [](ICanvas* iCanvas) {                          //
                        return std::make_unique<WavesScene>(iCanvas); //
                      });                                             //
}

_SpecialFXSamplesIndex::~_SpecialFXSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
