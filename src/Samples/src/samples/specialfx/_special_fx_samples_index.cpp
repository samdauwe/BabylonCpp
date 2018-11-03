#include <babylon/samples/specialfx/_special_fx_samples_index.h>

#include <babylon/samples/specialfx/environment_scene.h>
#include <babylon/samples/specialfx/fireworks_with_shader_code_scene.h>
#include <babylon/samples/specialfx/fog_scene.h>
#include <babylon/samples/specialfx/fresnel_scene.h>
#include <babylon/samples/specialfx/infinite_loader_scene.h>
#include <babylon/samples/specialfx/lens_flares_scene.h>
#include <babylon/samples/specialfx/realtime_reflection_scene.h>
#include <babylon/samples/specialfx/realtime_refraction_scene.h>
#include <babylon/samples/specialfx/refraction_and_reflection_dds_scene.h>
#include <babylon/samples/specialfx/refraction_and_reflection_scene.h>
#include <babylon/samples/specialfx/simple_post_process_render_pipeline_scene.h>
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
  // Lens Flares Scene
  _samples["LensFlaresScene"]
    = std::make_tuple(true,                                                //
                      [](ICanvas* iCanvas) {                               //
                        return std::make_unique<LensFlaresScene>(iCanvas); //
                      });                                                  //
  // Realtime Reflection Scene
  _samples["RealtimeReflectionScene"] = std::make_tuple(
    false,                                                       //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<RealtimeReflectionScene>(iCanvas); //
    });                                                          //
  // Realtime Refraction Scene
  _samples["RealtimeRefractionScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<RealtimeRefractionScene>(iCanvas); //
    });                                                          //
  // Refraction And Reflection DDS Scene
  _samples["RefractionAndReflectionDDSScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<RefractionAndReflectionDDSScene>(iCanvas); //
    });                                                                  //
  // Refraction And Reflection Scene
  _samples["RefractionAndReflectionScene"] = std::make_tuple(
    true,                                                             //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<RefractionAndReflectionScene>(iCanvas); //
    });                                                               //
  // Simple Post Process Render Pipeline Scene
  _samples["SimplePostProcessRenderPipelineScene"] = std::make_tuple(
    true,                                                                     //
    [](ICanvas* iCanvas) {                                                    //
      return std::make_unique<SimplePostProcessRenderPipelineScene>(iCanvas); //
    });                                                                       //
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
