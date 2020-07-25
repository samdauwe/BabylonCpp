#include <babylon/postprocesses/sub_surface_scattering_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/multi_render_target.h>
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

  onApplyObservable.add([this](Effect* effect, EventState & /*es*/) -> void {
    if (!_scene->prePassRenderer()) {
      BABYLON_LOG_ERROR("SubSurfaceScatteringPostProcess",
                        "PrePass needs to be enabled for subsurface scattering.");
      return;
    }
    const auto iTexelSize        = texelSize();
    const auto& prePassRenderer  = _scene->prePassRenderer();
    const auto prePassRTTextures = prePassRenderer->prePassRT->textures();
    effect->setFloat("metersPerUnit", prePassRenderer->subSurfaceConfiguration->metersPerUnit);
    effect->setFloat2("texelSize", iTexelSize.x, iTexelSize.y);
    effect->setTexture("irradianceSampler", prePassRTTextures[1]);
    effect->setTexture("depthSampler", prePassRTTextures[2]);
    effect->setTexture("albedoSampler", prePassRTTextures[3]);
    effect->setFloat2("viewportSize",
                      std::tan(_scene->activeCamera()->fov / 2.f)
                        * _scene->getEngine()->getAspectRatio(*_scene->activeCamera(), true),
                      std::tan(_scene->activeCamera()->fov / 2.f));
    effect->setArray3("diffusionS", prePassRenderer->subSurfaceConfiguration->ssDiffusionS());
    effect->setArray("diffusionD", prePassRenderer->subSurfaceConfiguration->ssDiffusionD());
    effect->setArray("filterRadii", prePassRenderer->subSurfaceConfiguration->ssFilterRadii());
  });
}

SubSurfaceScatteringPostProcess::~SubSurfaceScatteringPostProcess() = default;

} // end of namespace BABYLON
