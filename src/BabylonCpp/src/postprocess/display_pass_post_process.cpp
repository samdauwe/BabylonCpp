#include <babylon/postprocess/display_pass_post_process.h>

namespace BABYLON {

DisplayPassPostProcess::DisplayPassPostProcess(const std::string& _name,
                                               float ratio, Camera* camera,
                                               unsigned int samplingMode,
                                               Engine* engine, bool reusable)
    : PostProcess{_name,           "displayPass", {"passSampler"},
                  {"passSampler"}, ratio,         camera,
                  samplingMode,    engine,        reusable}
{
}

DisplayPassPostProcess::~DisplayPassPostProcess()
{
}

} // end of namespace BABYLON
