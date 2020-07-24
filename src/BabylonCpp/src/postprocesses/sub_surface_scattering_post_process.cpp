#include <babylon/postprocesses/sub_surface_scattering_post_process.h>

namespace BABYLON {

SubSurfaceScatteringPostProcess::SubSurfaceScatteringPostProcess(
  const std::string& iName, Scene* scene, const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable, unsigned int textureType)
    : PostProcess(iName, "subSurfaceScattering", {"texelSize", "viewportSize", "metersPerUnit"},
                  {"diffusionS", "diffusionD", "filterRadii", "irradianceSampler", "depthSampler",
                   "albedoSampler"},
                  options, camera, samplingMode.value_or(TextureConstants::BILINEAR_SAMPLINGMODE),
                  engine, reusable, nullptr, textureType, "postprocess", {}, true)
{
  _scene = scene;
}

SubSurfaceScatteringPostProcess::~SubSurfaceScatteringPostProcess() = default;

} // end of namespace BABYLON
