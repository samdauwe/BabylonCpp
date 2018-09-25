#include <babylon/samples/lights/_lights_samples_index.h>

#include <babylon/samples/lights/directional_light_scene.h>
#include <babylon/samples/lights/hemispheric_light_scene.h>
#include <babylon/samples/lights/light_projected_texture_scene.h>
#include <babylon/samples/lights/lights_scene.h>
#include <babylon/samples/lights/point_light_scene.h>
#include <babylon/samples/lights/spot_light_scene.h>

namespace BABYLON {
namespace Samples {

_LightsSamplesIndex::_LightsSamplesIndex()
{
  // -- Lights examples --

  // Directional Light Scene
  _samples["DirectionalLightScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<DirectionalLightScene>(iCanvas); //
    });                                                          //
  // Hemispheric Light Scene
  _samples["HemisphericLightScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<HemisphericLightScene>(iCanvas); //
    });                                                          //
  // Light Projected Texture Scene
  _samples["LightProjectedTextureScene"] = std::make_tuple(
    true,                                                             //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<LightProjectedTextureScene>(iCanvas); //
    });                                                               //
  // Lights Scene
  _samples["LightsScene"]
    = std::make_tuple(true,                                              //
                        [](ICanvas* iCanvas) {                             //
                          return std::make_unique<LightsScene>(iCanvas); //
                        });                                                //
  // Point light Scene
  _samples["PointLightScene"] = std::make_tuple(
    true,                                                  //
    [](ICanvas* iCanvas) {                                 //
      return std::make_unique<PointLightScene>(iCanvas); //
    });                                                    //
  // Spot light Scene
  _samples["SpotLightScene"]
    = std::make_tuple(true,                                                 //
                        [](ICanvas* iCanvas) {                                //
                          return std::make_unique<SpotLightScene>(iCanvas); //
                        });                                                   //
}

_LightsSamplesIndex::~_LightsSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
