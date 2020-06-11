#include <babylon/postprocesses/depth_of_field_merge_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

DepthOfFieldMergePostProcess::DepthOfFieldMergePostProcess(
  const std::string& iName, const PostProcessPtr& originalFromInput,
  const PostProcessPtr& circleOfConfusion, const std::vector<PostProcessPtr>& iBlurSteps,
  const std::variant<float, PostProcessOptions>& options, const CameraPtr& camera,
  const std::optional<unsigned int>& samplingMode, Engine* engine, bool reusable,
  unsigned int textureType, bool blockCompilation)
    : PostProcess{iName,        "depthOfFieldMerge",
                  {},           {"circleOfConfusionSampler", "blurStep0", "blurStep1", "blurStep2"},
                  options,      camera,
                  samplingMode, engine,
                  reusable,     "",
                  textureType,  "",
                  {},           true}
    , blurSteps{iBlurSteps}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    effect->setTextureFromPostProcessOutput("circleOfConfusionSampler", circleOfConfusion);
    effect->setTextureFromPostProcess("textureSampler", originalFromInput);
    for (auto& blurStep : blurSteps)
      effect->setTextureFromPostProcessOutput("blurStep" + std::to_string(blurSteps.size() - 1),
                                              blurStep);
  });

  // updateEffect() is a virtual method, and thus cannot be called in the constructor
  // it is instead called inside DepthOfFieldMergePostProcess::New(...)
  // if (!blockCompilation)
  // updateEffect();
  (void)blockCompilation;
}

DepthOfFieldMergePostProcess::~DepthOfFieldMergePostProcess() = default;

void DepthOfFieldMergePostProcess::updateEffect(
  const std::string& defines, const std::vector<std::string>& uniforms,
  const std::vector<std::string>& samplers,
  const std::unordered_map<std::string, unsigned int>& indexParameters,
  const std::function<void(Effect* effect)>& onCompiled,
  const std::function<void(Effect* effect, const std::string& errors)>& onError,
  const std::string& /*vertexUrl*/, const std::string& /*fragmentUrl*/)
{
  auto idefines = defines;
  if (!defines.empty()) {
    idefines = "";
    idefines += "#define BLUR_LEVEL " + std::to_string(blurSteps.size() - 1) + "\n";
  }
  PostProcess::updateEffect(idefines, uniforms, samplers, indexParameters, onCompiled, onError);
}

} // end of namespace BABYLON
