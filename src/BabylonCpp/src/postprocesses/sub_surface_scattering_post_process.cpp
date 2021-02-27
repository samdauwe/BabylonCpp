#include <babylon/postprocesses/sub_surface_scattering_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/pre_pass_render_target.h>
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/sub_surface_configuration.h>

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

  updateEffect();

  onApplyObservable.add([this](Effect* effect, EventState& /*es*/) -> void {
    if (!_scene->prePassRenderer() || !_scene->subSurfaceConfiguration()) {
      BABYLON_LOG_ERROR(
        "SubSurfaceScatteringPostProcess",
        "PrePass and subsurface configuration needs to be enabled for subsurface scattering.");
      return;
    }
    const auto iTexelSize = texelSize();
    effect->setFloat("metersPerUnit", _scene->subSurfaceConfiguration()->metersPerUnit);
    effect->setFloat2("texelSize", iTexelSize.x, iTexelSize.y);
    effect->setTexture(
      "irradianceSampler",
      _scene->prePassRenderer()->getRenderTarget()->textures()[_scene->prePassRenderer()->getIndex(
        Constants::PREPASS_IRRADIANCE_TEXTURE_TYPE)]);
    effect->setTexture(
      "depthSampler",
      _scene->prePassRenderer()
        ->getRenderTarget()
        ->textures()[_scene->prePassRenderer()->getIndex(Constants::PREPASS_DEPTH_TEXTURE_TYPE)]);
    effect->setTexture(
      "albedoSampler",
      _scene->prePassRenderer()
        ->getRenderTarget()
        ->textures()[_scene->prePassRenderer()->getIndex(Constants::PREPASS_ALBEDO_TEXTURE_TYPE)]);
    effect->setFloat2("viewportSize",
                      std::tan(_scene->activeCamera()->fov / 2.f)
                        * _scene->getEngine()->getAspectRatio(*_scene->activeCamera(), true),
                      std::tan(_scene->activeCamera()->fov / 2.f));
    effect->setArray3("diffusionS", _scene->subSurfaceConfiguration()->ssDiffusionS());
    effect->setArray("diffusionD", _scene->subSurfaceConfiguration()->ssDiffusionD());
    effect->setArray("filterRadii", _scene->subSurfaceConfiguration()->ssFilterRadii());
  });
}

SubSurfaceScatteringPostProcess::~SubSurfaceScatteringPostProcess() = default;

std::string SubSurfaceScatteringPostProcess::getClassName() const
{
  return "SubSurfaceScatteringPostProcess";
}

} // end of namespace BABYLON
