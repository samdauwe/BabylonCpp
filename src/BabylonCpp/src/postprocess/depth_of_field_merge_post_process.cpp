#include <babylon/postprocess/depth_of_field_merge_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

DepthOfFieldMergePostProcess::DepthOfFieldMergePostProcess(
  const string_t& name, PostProcess* originalFromInput,
  PostProcess* circleOfConfusion, const vector_t<PostProcess*>& iBlurSteps,
  const Variant<float, PostProcessOptions>& options, const CameraPtr& camera,
  unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType, bool blockCompilation)
    : PostProcess{name,
                  "depthOfFieldMerge",
                  {},
                  {"circleOfConfusionSampler", "blurStep0", "blurStep1",
                   "blurStep2"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "",
                  textureType,
                  "",
                  {},
                  true}
    , blurSteps{iBlurSteps}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    effect->setTextureFromPostProcessOutput("circleOfConfusionSampler",
                                            circleOfConfusion);
    effect->setTextureFromPostProcess("textureSampler", originalFromInput);
    for (size_t index = 0; index < blurSteps.size(); ++index)
      effect->setTextureFromPostProcessOutput(
        "blurStep" + std::to_string(blurSteps.size() - 1), blurSteps[index]);
  });

  if (!blockCompilation) {
    updateEffect();
  }
}

DepthOfFieldMergePostProcess::~DepthOfFieldMergePostProcess()
{
}

void DepthOfFieldMergePostProcess::updateEffect(
  const string_t& defines, const vector_t<string_t>& uniforms,
  const vector_t<string_t>& samplers,
  const unordered_map_t<string_t, unsigned int>& indexParameters,
  const std::function<void(Effect* effect)>& onCompiled,
  const std::function<void(Effect* effect, const string_t& errors)>& onError)
{
  auto _defines = defines;
  if (!defines.empty()) {
    _defines = "";
    _defines
      += "#define BLUR_LEVEL " + std::to_string(blurSteps.size() - 1) + "\n";
  }
  PostProcess::updateEffect(_defines, uniforms, samplers, indexParameters,
                            onCompiled, onError);
}

} // end of namespace BABYLON
