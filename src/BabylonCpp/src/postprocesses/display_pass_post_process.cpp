#include <babylon/postprocesses/display_pass_post_process.h>

namespace BABYLON {

DisplayPassPostProcess::DisplayPassPostProcess(const std::string& iName,
                                               float ratio,
                                               const CameraPtr& camera,
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
