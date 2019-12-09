#include <babylon/postprocesses/circle_of_confusion_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/render_target_texture.h>

namespace BABYLON {

CircleOfConfusionPostProcess::CircleOfConfusionPostProcess(
  const std::string& iName, RenderTargetTexture* depthTexture,
  const std::variant<float, PostProcessOptions>& options, const CameraPtr& camera,
  const std::optional<unsigned int>& samplingMode, Engine* engine, bool reusable,
  unsigned int textureType, bool blockCompilation)
    : PostProcess{iName,
                  "circleOfConfusion",
                  {"cameraMinMaxZ", "focusDistance", "cocPrecalculation"},
                  {"depthSampler"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  nullptr,
                  textureType,
                  "",
                  {},
                  blockCompilation}
    , lensSize{50.f}
    , fStop{1.4f}
    , focusDistance{2000.f}
    , focalLength{50.f}
    , depthTexture{this, &CircleOfConfusionPostProcess::set_depthTexture}
    , _depthTexture{depthTexture}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    if (!_depthTexture) {
      BABYLON_LOG_WARN("CircleOfConfusionPostProcess",
                       "No depth texture set on CircleOfConfusionPostProcess")
      return;
    }
    effect->setTexture("depthSampler", _depthTexture);

    // Circle of confusion calculation, See
    // https://developer.nvidia.com/gpugems/GPUGems/gpugems_ch23.html
    float aperture          = lensSize / fStop;
    float cocPrecalculation = ((aperture * focalLength)
                               / ((focusDistance - focalLength))); // * ((focusDistance -
                                                                   // pixelDistance)/pixelDistance)
                                                                   // [This part is done in shader]

    effect->setFloat("focusDistance", focusDistance);
    effect->setFloat("cocPrecalculation", cocPrecalculation);

    if (_depthTexture->activeCamera) {
      effect->setFloat2("cameraMinMaxZ", _depthTexture->activeCamera->minZ,
                        _depthTexture->activeCamera->maxZ);
    }
  });
}

CircleOfConfusionPostProcess::~CircleOfConfusionPostProcess() = default;

void CircleOfConfusionPostProcess::set_depthTexture(const RenderTargetTexturePtr& value)
{
  _depthTexture = value;
}

} // end of namespace BABYLON
