#include <babylon/rendering/sub_surface_configuration.h>

#include <cmath>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/postprocesses/sub_surface_scattering_post_process.h>
#include <babylon/rendering/sub_surface_scene_component.h>

namespace BABYLON {

SubSurfaceConfiguration::SubSurfaceConfiguration(Scene* scene)
    : postProcess{nullptr}
    , ssDiffusionS{this, &SubSurfaceConfiguration::get_ssDiffusionS}
    , ssDiffusionD{this, &SubSurfaceConfiguration::get_ssDiffusionD}
    , ssFilterRadii{this, &SubSurfaceConfiguration::get_ssFilterRadii}
    , metersPerUnit{1.f}
{
  enabled = false;

  // Adding default diffusion profile
  addDiffusionProfile(Color3(1.f, 1.f, 1.f));
  _scene = scene;

  // Register the G Buffer component to the scene.
  auto component = scene->_getComponent(SceneComponentConstants::NAME_SUBSURFACE);
  if (!component) {
    component = SubSurfaceSceneComponent::New(scene);
    scene->_addComponent(component);
  }
}

std::string SubSurfaceConfiguration::name() const
{
  return SceneComponentConstants::NAME_SUBSURFACE;
}

std::vector<uint32_t> SubSurfaceConfiguration::texturesRequired() const
{
  return {
    Constants::PREPASS_DEPTHNORMAL_TEXTURE_TYPE, //
    Constants::PREPASS_ALBEDO_TEXTURE_TYPE,      //
    Constants::PREPASS_COLOR_TEXTURE_TYPE,       //
    Constants::PREPASS_IRRADIANCE_TEXTURE_TYPE,  //
  };
}

Float32Array& SubSurfaceConfiguration::get_ssDiffusionS()
{
  return _ssDiffusionS;
}

Float32Array& SubSurfaceConfiguration::get_ssDiffusionD()
{
  return _ssDiffusionD;
}

Float32Array& SubSurfaceConfiguration::get_ssFilterRadii()
{
  return _ssFilterRadii;
}

size_t SubSurfaceConfiguration::addDiffusionProfile(const Color3& color)
{
  if (_ssDiffusionD.size() >= 5) {
    // We only suppport 5 diffusion profiles
    BABYLON_LOG_ERROR("SubSurfaceConfiguration",
                      "You already reached the maximum number of diffusion profiles.");
    return 0; // default profile
  }

  // Do not add doubles
  for (size_t i = 0; i < _ssDiffusionS.size() / 3; ++i) {
    if (_ssDiffusionS[i * 3] == color.r        //
        && _ssDiffusionS[i * 3 + 1] == color.g //
        && _ssDiffusionS[i * 3 + 2] == color.b) {
      return i;
    }
  }

  stl_util::concat(_ssDiffusionS, {color.r, color.b, color.g});
  _ssDiffusionD.emplace_back(std::max(std::max(color.r, color.b), color.g));
  _ssFilterRadii.emplace_back(getDiffusionProfileParameters(color));
  ssDiffusionProfileColors.emplace_back(color);

  return _ssDiffusionD.size() - 1;
}

PostProcessPtr SubSurfaceConfiguration::createPostProcess()
{
  postProcess = SubSurfaceScatteringPostProcess::New("subSurfaceScattering", _scene, 1.f, nullptr,
                                                     std::nullopt, _scene->getEngine());
  postProcess->autoClear = false;

  return postProcess;
}

void SubSurfaceConfiguration::clearAllDiffusionProfiles()
{
  _ssDiffusionD            = {};
  _ssDiffusionS            = {};
  _ssFilterRadii           = {};
  ssDiffusionProfileColors = {};
}

void SubSurfaceConfiguration::dispose()
{
  clearAllDiffusionProfiles();
  postProcess->dispose();
}

float SubSurfaceConfiguration::getDiffusionProfileParameters(const Color3& color) const
{
  const auto cdf                   = 0.997f;
  const auto maxScatteringDistance = std::max(std::max(color.r, color.g), color.b);

  return _sampleBurleyDiffusionProfile(cdf, maxScatteringDistance);
}

float SubSurfaceConfiguration::_sampleBurleyDiffusionProfile(float u, float rcpS) const
{
  u = 1.f - u; // Convert CDF to CCDF

  const auto g = 1.f + (4 * u) * (2.f * u + std::sqrt(1.f + (4.f * u) * u));
  const auto n = std::pow(g, -1.f / 3.f); // g^(-1/3)
  const auto p = (g * n) * n;             // g^(+1/3)
  const auto c = 1.f + p + n;             // 1 + g^(+1/3) + g^(-1/3)
  const auto x = 3.f * std::log(c / (4.f * u));

  return x * rcpS;
}

} // end of namespace BABYLON
