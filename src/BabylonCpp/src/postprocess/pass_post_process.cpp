#include <babylon/postprocess/pass_post_process.h>

namespace BABYLON {

PassPostProcess::PassPostProcess(const std::string& iName, float ratio,
                                 Camera* camera, unsigned int samplingMode,
                                 Engine* engine, bool reusable)
    : PostProcess{iName,  "pass",       {},     {},      ratio,
                  camera, samplingMode, engine, reusable}
{
}

PassPostProcess::~PassPostProcess()
{
}

} // end of namespace BABYLON
