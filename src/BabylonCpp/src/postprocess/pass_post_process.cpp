#include <babylon/postprocess/pass_post_process.h>

#include <babylon/core/variant.h>

namespace BABYLON {

PassPostProcess::PassPostProcess(const string_t& iName, float ratio,
                                 const CameraPtr& camera,
                                 unsigned int samplingMode, Engine* engine,
                                 bool reusable, unsigned int textureType,
                                 bool blockCompilation)
    : PostProcess{iName,
                  "pass",
                  {},
                  {},
                  ToVariant<float, PostProcessOptions>(ratio),
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "",
                  textureType,
                  "",
                  {},
                  blockCompilation}
{
}

PassPostProcess::~PassPostProcess()
{
}

} // end of namespace BABYLON
