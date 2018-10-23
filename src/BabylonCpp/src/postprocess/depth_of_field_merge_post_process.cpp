#include <babylon/postprocess/depth_of_field_merge_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

DepthOfFieldMergePostProcess::DepthOfFieldMergePostProcess(
  const std::string& name, PostProcess* originalFromInput,
  PostProcess* circleOfConfusion, const std::vector<PostProcess*>& iBlurSteps,
  const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
  bool reusable, unsigned int textureType, bool blockCompilation)
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
  const std::string& defines, const std::vector<std::string>& uniforms,
  const std::vector<std::string>& samplers,
  const std::unordered_map<std::string, unsigned int>& indexParameters,
  const std::function<void(Effect* effect)>& onCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& onError)
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
