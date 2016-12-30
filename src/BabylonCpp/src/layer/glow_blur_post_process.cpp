#include <babylon/layer/glow_blur_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

GlowBlurPostProcess::GlowBlurPostProcess(const std::string& name,
                                         const Vector2& iDirection,
                                         float iBlurWidth, float options,
                                         Camera* camera,
                                         unsigned int samplingMode,
                                         Engine* engine, bool reusable)
    : PostProcess(name, "glowBlurPostProcess",
                  {"screenSize", "direction", "blurWidth"}, {}, options, camera,
                  samplingMode, engine, reusable)
    , direction{iDirection}
    , blurWidth{iBlurWidth}
{
  onApplyObservable.add([this](Effect* effect) {
    effect->setFloat2("screenSize", width, height);
    effect->setVector2("direction", direction);
    effect->setFloat("blurWidth", blurWidth);
  });
}

GlowBlurPostProcess::GlowBlurPostProcess(
  const std::string& name, const Vector2& iDirection, float iBlurWidth,
  const PostProcessOptions& options, Camera* camera, unsigned int samplingMode,
  Engine* engine, bool reusable)
    : PostProcess(name, "glowBlurPostProcess",
                  {"screenSize", "direction", "blurWidth"}, {}, options, camera,
                  samplingMode, engine, reusable)
    , direction{iDirection}
    , blurWidth{iBlurWidth}
{
  onApplyObservable.add([this](Effect* effect) {
    effect->setFloat2("screenSize", width, height);
    effect->setVector2("direction", direction);
    effect->setFloat("blurWidth", blurWidth);
  });
}

GlowBlurPostProcess::~GlowBlurPostProcess()
{
}

} // end of namespace BABYLON
