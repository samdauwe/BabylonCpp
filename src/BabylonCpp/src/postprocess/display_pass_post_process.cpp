#include <babylon/postprocess/display_pass_post_process.h>

#include <babylon/core/variant.h>

namespace BABYLON {

DisplayPassPostProcess::DisplayPassPostProcess(const string_t& iName,
                                               float ratio, Camera* camera,
                                               unsigned int samplingMode,
                                               Engine* engine, bool reusable)
    : PostProcess{iName,
                  "displayPass",
                  {"passSampler"},
                  {"passSampler"},
                  ToVariant<float, PostProcessOptions>(ratio),
                  camera,
                  samplingMode,
                  engine,
                  reusable}
{
}

DisplayPassPostProcess::~DisplayPassPostProcess()
{
}

} // end of namespace BABYLON
