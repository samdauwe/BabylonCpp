#include <babylon/samples/specialfx/_special_fx_samples_index.h>

#include <babylon/samples/specialfx/convolution_post_process_scene.h>
#include <babylon/samples/specialfx/dawn_bringer_scene.h>
#include <babylon/samples/specialfx/environment_scene.h>
#include <babylon/samples/specialfx/fade_in_out_scene.h>
#include <babylon/samples/specialfx/fireworks_with_shader_code_scene.h>
#include <babylon/samples/specialfx/fog_scene.h>
#include <babylon/samples/specialfx/fresnel_scene.h>
#include <babylon/samples/specialfx/glass_wubble_ball_scene.h>
#include <babylon/samples/specialfx/heat_wave_scene.h>
#include <babylon/samples/specialfx/infinite_loader_scene.h>
#include <babylon/samples/specialfx/kernel_based_blur_scene.h>
#include <babylon/samples/specialfx/lens_flares_scene.h>
#include <babylon/samples/specialfx/points_cloud_scene.h>
#include <babylon/samples/specialfx/portals_scene.h>
#include <babylon/samples/specialfx/realtime_reflection_scene.h>
#include <babylon/samples/specialfx/realtime_refraction_scene.h>
#include <babylon/samples/specialfx/refraction_and_reflection_dds_scene.h>
#include <babylon/samples/specialfx/refraction_and_reflection_scene.h>
#include <babylon/samples/specialfx/rgb_shift_glitch_scene.h>
#include <babylon/samples/specialfx/simple_post_process_render_pipeline_scene.h>
#include <babylon/samples/specialfx/sprites_scene.h>
#include <babylon/samples/specialfx/volumetric_light_scattering_scene.h>
#include <babylon/samples/specialfx/wave_of_sprites_scene.h>
#include <babylon/samples/specialfx/waves_scene.h>

namespace BABYLON {
namespace Samples {

_SpecialFXSamplesIndex::_SpecialFXSamplesIndex()
{
  // -- Special FX examples --

  // Convolution Post-process Scene
  _samples["ConvolutionPostProcessScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<ConvolutionPostProcessScene>(iCanvas); //
    });                                                              //
  // Dawn Bringer Scene
  _samples["DawnBringerScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<DawnBringerScene>(iCanvas); //
                      });                                                   //
  // Fade In / Out Scene
  _samples["FadeInOutScene"]
    = std::make_tuple(true,                                               //
                      [](ICanvas* iCanvas) {                              //
                        return std::make_unique<FadeInOutScene>(iCanvas); //
                      });                                                 //
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
  // Glass Wubble Ball Scene
  _samples["GlassWubbleBallScene"] = std::make_tuple(
    true,                                                     //
    [](ICanvas* iCanvas) {                                    //
      return std::make_unique<GlassWubbleBallScene>(iCanvas); //
    });                                                       //
  // Heat wave Scene
  _samples["HeatWaveScene"]
    = std::make_tuple(true,                                              //
                      [](ICanvas* iCanvas) {                             //
                        return std::make_unique<HeatWaveScene>(iCanvas); //
                      });                                                //
  // Infinite Loader Scene
  _samples["InfiniteLoaderScene"] = std::make_tuple(
    false,                                                   //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<InfiniteLoaderScene>(iCanvas); //
    });                                                      //
  // Kernel Based Blur Scene
  _samples["KernelBasedBlurScene"] = std::make_tuple(
    true,                                                     //
    [](ICanvas* iCanvas) {                                    //
      return std::make_unique<KernelBasedBlurScene>(iCanvas); //
    });                                                       //
  // Lens Flares Scene
  _samples["LensFlaresScene"]
    = std::make_tuple(true,                                                //
                      [](ICanvas* iCanvas) {                               //
                        return std::make_unique<LensFlaresScene>(iCanvas); //
                      });                                                  //
  // Points Cloud Scene
  _samples["PointsCloudScene"]
    = std::make_tuple(false,                                                //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<PointsCloudScene>(iCanvas); //
                      });                                                   //
  // Portals Scene
  _samples["PortalsScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<PortalsScene>(iCanvas); //
                      });                                               //
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
  // Refraction And Reflection DDS Scene
  _samples["RefractionAndReflectionDDSScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<RefractionAndReflectionDDSScene>(iCanvas); //
    });                                                                  //
  // RGB Shift Glitch Scene
  _samples["RGBShiftGlitchScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<RGBShiftGlitchScene>(iCanvas); //
    });                                                      //
  // Simple Post Process Render Pipeline Scene
  _samples["SimplePostProcessRenderPipelineScene"] = std::make_tuple(
    true,                                                                     //
    [](ICanvas* iCanvas) {                                                    //
      return std::make_unique<SimplePostProcessRenderPipelineScene>(iCanvas); //
    });                                                                       //
  // Sprites Scene
  _samples["SpritesScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<SpritesScene>(iCanvas); //
                      });                                               //
  // Wave Of Sprites Scene
  _samples["WaveOfSpritesScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<WaveOfSpritesScene>(iCanvas); //
                      });                                                     //
  // Waves Scene
  _samples["WavesScene"]
    = std::make_tuple(true,                                           //
                      [](ICanvas* iCanvas) {                          //
                        return std::make_unique<WavesScene>(iCanvas); //
                      });                                             //
  // Volumetric Light Scattering Scene
  _samples["VolumetricLightScatteringScene"] = std::make_tuple(
    false,                                                              //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<VolumetricLightScatteringScene>(iCanvas); //
    });                                                                 //
}

_SpecialFXSamplesIndex::~_SpecialFXSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
