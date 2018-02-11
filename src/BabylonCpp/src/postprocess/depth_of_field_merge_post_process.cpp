#include <babylon/postprocess/depth_of_field_merge_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

DepthOfFieldMergePostProcess::DepthOfFieldMergePostProcess(
  const string_t& name, PostProcess* original, PostProcess* circleOfConfusion,
  const vector_t<PostProcess*>& blurSteps,
  const Variant<float, PostProcessOptions>& options, Camera* camera,
  unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType)
    : PostProcess{name,
                  "depthOfFieldMerge",
                  {},
                  {"circleOfConfusionSampler", "originalSampler", "blurStep1",
                   "blurStep2"},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "#define BLUR_LEVEL " + ::std::to_string(blurSteps.size())
                    + "\n",
                  textureType}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    effect->setTextureFromPostProcess("circleOfConfusionSampler",
                                      circleOfConfusion);
    effect->setTextureFromPostProcess("originalSampler", original);
    for (size_t index = 0; index < blurSteps.size(); ++index)
      effect->setTextureFromPostProcess(
        "blurStep" + ::std::to_string(index + 1), blurSteps[index]);
  });
}

DepthOfFieldMergePostProcess::~DepthOfFieldMergePostProcess()
{
}

} // end of namespace BABYLON
