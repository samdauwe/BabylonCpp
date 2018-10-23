#include <babylon/postprocess/pass_post_process.h>

namespace BABYLON {

PassPostProcess::PassPostProcess(const std::string& iName, float ratio,
                                 const CameraPtr& camera,
                                 unsigned int samplingMode, Engine* engine,
                                 bool reusable, unsigned int textureType,
                                 bool blockCompilation)
    : PostProcess{
        iName,  "pass",   {}, {},          ratio, camera, samplingMode,
        engine, reusable, "", textureType, "",    {},     blockCompilation}
{
}

PassPostProcess::~PassPostProcess()
{
}

} // end of namespace BABYLON
