#include <babylon/postprocesses/glow_blur_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

GlowBlurPostProcess::GlowBlurPostProcess(
  const std::string& name, const Vector2& iDirection, float iKernel,
  const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
  bool reusable)
    : PostProcess(name, "glowBlurPostProcess",
                  {"screenSize", "direction", "blurWidth"}, {}, options, camera,
                  samplingMode, engine, reusable)
    , direction{iDirection}
    , kernel{iKernel}
{
  onApplyObservable.add([this](Effect* effect, EventState&) {
    effect->setFloat2("screenSize", static_cast<float>(width),
                      static_cast<float>(height));
    effect->setVector2("direction", direction);
    effect->setFloat("blurWidth", kernel);
  });
}

GlowBlurPostProcess::~GlowBlurPostProcess() = default;

} // end of namespace BABYLON
