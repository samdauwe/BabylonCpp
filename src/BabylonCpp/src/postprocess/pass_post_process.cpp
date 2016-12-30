#include <babylon/postprocess/pass_post_process.h>

namespace BABYLON {

PassPostProcess::PassPostProcess(const std::string& _name, float ratio,
                                 Camera* camera, unsigned int samplingMode,
                                 Engine* engine, bool reusable)
    : PostProcess{_name,  "pass",       {},     {},      ratio,
                  camera, samplingMode, engine, reusable}
{
}

PassPostProcess::~PassPostProcess()
{
}

} // end of namespace BABYLON
