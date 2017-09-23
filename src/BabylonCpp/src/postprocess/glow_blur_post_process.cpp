#include <babylon/postprocess/glow_blur_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

GlowBlurPostProcess::GlowBlurPostProcess(
  const string_t& name, const Vector2& iDirection, float iKernel, float options,
  Camera* camera, unsigned int samplingMode, Engine* engine, bool reusable)
    : PostProcess(name, "glowBlurPostProcess",
                  {"screenSize", "direction", "blurWidth"}, {}, options, camera,
                  samplingMode, engine, reusable)
    , direction{iDirection}
    , kernel{iKernel}
{
  onApplyObservable.add([this](Effect* effect, EventState&) {
    effect->setFloat2("screenSize", width, height);
    effect->setVector2("direction", direction);
    effect->setFloat("blurWidth", kernel);
  });
}

GlowBlurPostProcess::GlowBlurPostProcess(
  const string_t& name, const Vector2& iDirection, float iKernel,
  const PostProcessOptions& options, Camera* camera, unsigned int samplingMode,
  Engine* engine, bool reusable)
    : PostProcess(name, "glowBlurPostProcess",
                  {"screenSize", "direction", "blurWidth"}, {}, options, camera,
                  samplingMode, engine, reusable)
    , direction{iDirection}
    , kernel{iKernel}
{
  onApplyObservable.add([this](Effect* effect, EventState&) {
    effect->setFloat2("screenSize", width, height);
    effect->setVector2("direction", direction);
    effect->setFloat("blurWidth", kernel);
  });
}

GlowBlurPostProcess::~GlowBlurPostProcess()
{
}

} // end of namespace BABYLON
