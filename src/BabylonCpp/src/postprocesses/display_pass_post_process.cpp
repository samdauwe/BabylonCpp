#include <babylon/postprocesses/display_pass_post_process.h>

namespace BABYLON {

DisplayPassPostProcess::DisplayPassPostProcess(
  const std::string& iName, const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable)
    : PostProcess{iName,  "displayPass", {"passSampler"}, {"passSampler"}, options,
                  camera, samplingMode,  engine,          reusable}
{
}

DisplayPassPostProcess::~DisplayPassPostProcess() = default;

std::string DisplayPassPostProcess::getClassName() const
{
  return "DisplayPassPostProcess";
}

DisplayPassPostProcessPtr DisplayPassPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                                         const CameraPtr& /*targetCamera*/,
                                                         Scene* /*scene*/,
                                                         const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
