#include <babylon/postprocesses/pass_post_process.h>

namespace BABYLON {

PassPostProcess::PassPostProcess(const std::string& iName, float ratio, const CameraPtr& camera,
                                 unsigned int samplingMode, Engine* engine, bool reusable,
                                 unsigned int textureType, bool blockCompilation)
    : PostProcess{iName,  "pass",   {}, {},          ratio, camera, samplingMode,
                  engine, reusable, "", textureType, "",    {},     blockCompilation}
{
}

PassPostProcess::~PassPostProcess() = default;

std::string PassPostProcess::getClassName() const
{
  return "PassPostProcess";
}

PassPostProcessPtr PassPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                           const CameraPtr& /*targetCamera*/, Scene* /*scene*/,
                                           const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
