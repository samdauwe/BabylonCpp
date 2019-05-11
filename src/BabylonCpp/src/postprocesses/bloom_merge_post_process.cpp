#include <babylon/postprocesses/bloom_merge_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

BloomMergePostProcess::BloomMergePostProcess(
  const std::string& iName, const PostProcessPtr& iOriginalFromInput,
  const PostProcessPtr& iBlurred, float iWeight,
  const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, unsigned int samplingMode, Engine* engine,
  bool reusable, unsigned int textureType, bool blockCompilation)
    : PostProcess(iName, "bloomMerge", {"bloomWeight"},
                  {"circleOfConfusionSampler", "blurStep0", "blurStep1",
                   "blurStep2", "bloomBlur"},
                  options, camera, samplingMode, engine, reusable, "",
                  textureType, "", {}, true)
    , originalFromInput{iOriginalFromInput}
    , blurred{iBlurred}
    , weight{iWeight}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    effect->setTextureFromPostProcess("textureSampler",
                                      originalFromInput.get());
    effect->setTextureFromPostProcessOutput("bloomBlur", blurred.get());
    effect->setFloat("bloomWeight", weight);
  });

  if (!blockCompilation) {
    updateEffect();
  }
}

BloomMergePostProcess::~BloomMergePostProcess()
{
}

} // end of namespace BABYLON
