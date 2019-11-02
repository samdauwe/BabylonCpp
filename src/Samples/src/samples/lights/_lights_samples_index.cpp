#include <babylon/samples/lights/_lights_samples_index.h>

#include <babylon/samples/lights/directional_light_scene.h>
#include <babylon/samples/lights/hemispheric_light_scene.h>
#include <babylon/samples/lights/intersecting_lights_colors_scene.h>
#include <babylon/samples/lights/light_projected_texture_scene.h>
#include <babylon/samples/lights/lights_scene.h>
#include <babylon/samples/lights/point_light_scene.h>
#include <babylon/samples/lights/simultaneous_lights_scene.h>
#include <babylon/samples/lights/spot_light_scene.h>

namespace BABYLON {
namespace Samples {

_LightsSamplesIndex::_LightsSamplesIndex()
{
  // -- Lights examples --

  // Directional Light Scene
  _samples["DirectionalLightScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<DirectionalLightScene>(iCanvas);
  };
  // Hemispheric Light Scene
  _samples["HemisphericLightScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<HemisphericLightScene>(iCanvas);
  };
  // Intersecting Lights Colors Scene
  _samples["IntersectingLightsColorsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<IntersectingLightsColorsScene>(iCanvas);
  };
  // Light Projected Texture Scene
  _samples["LightProjectedTextureScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<LightProjectedTextureScene>(iCanvas);
  };
  // Lights Scene
  _samples["LightsScene"]
    = [](ICanvas* iCanvas) { return std::make_unique<LightsScene>(iCanvas); };
  // Point light Scene
  _samples["PointLightScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PointLightScene>(iCanvas);
  };
  // Simultaneous light Scene
  _samples["SimultaneousLightsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimultaneousLightsScene>(iCanvas);
  };
  // Spot light Scene
  _samples["SpotLightScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SpotLightScene>(iCanvas);
  };
}
} // namespace Samples
} // namespace BABYLON
