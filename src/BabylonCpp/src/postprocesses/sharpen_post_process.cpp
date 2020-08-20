#include <babylon/postprocesses/sharpen_post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

SharpenPostProcess::SharpenPostProcess(const std::string& iName,
                                       const std::variant<float, PostProcessOptions>& options,
                                       const CameraPtr& camera,
                                       const std::optional<unsigned int>& samplingMode,
                                       Engine* engine, bool reusable, unsigned int textureType,
                                       bool blockCompilation)
    : PostProcess{iName,        "sharpen",       {"sharpnessAmounts", "screenSize"},
                  {},           options,         camera,
                  samplingMode, engine,          reusable,
                  "",           textureType,     "",
                  {},           blockCompilation}
    , colorAmount{1.f}
    , edgeAmount{0.3f}
{
  onApply = [this](Effect* effect, EventState& /*ev*/) {
    effect->setFloat2("screenSize", static_cast<float>(width), static_cast<float>(height));
    effect->setFloat2("sharpnessAmounts", edgeAmount, colorAmount);
  };
}

SharpenPostProcess::~SharpenPostProcess() = default;

std::string SharpenPostProcess::getClassName() const
{
  return "SharpenPostProcess";
}

SharpenPostProcessPtr SharpenPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                                 const CameraPtr& /*targetCamera*/,
                                                 Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
