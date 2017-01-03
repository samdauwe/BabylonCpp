#include <babylon/postprocess/display_pass_post_process.h>

namespace BABYLON {

DisplayPassPostProcess::DisplayPassPostProcess(const std::string& iName,
                                               float ratio, Camera* camera,
                                               unsigned int samplingMode,
                                               Engine* engine, bool reusable)
    : PostProcess{iName,           "displayPass", {"passSampler"},
                  {"passSampler"}, ratio,         camera,
                  samplingMode,    engine,        reusable}
{
}

DisplayPassPostProcess::~DisplayPassPostProcess()
{
}

} // end of namespace BABYLON
