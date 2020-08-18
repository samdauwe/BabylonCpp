#include <babylon/postprocesses/fxaa_post_process.h>

#include <babylon/engines/engine.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/misc/string_tools.h>

namespace BABYLON {

FxaaPostProcess::FxaaPostProcess(const std::string& iName, float ratio, const CameraPtr& camera,
                                 const std::optional<unsigned int>& samplingMode, Engine* engine,
                                 bool reusable, unsigned int textureType)
    : PostProcess{iName,
                  "fxaa",
                  {"texelSize"},
                  {},
                  ratio,
                  camera,
                  samplingMode.value_or(TextureConstants::BILINEAR_SAMPLINGMODE),
                  engine,
                  reusable,
                  "",
                  textureType,
                  "fxaa",
                  {},
                  true}
{
  const auto defines = _getDefines();
  updateEffect(defines);

  onApplyObservable.add([this](Effect* effect, EventState&) {
    auto _texelSize = texelSize();
    effect->setFloat2("texelSize", _texelSize.x, _texelSize.y);
  });
}

FxaaPostProcess::~FxaaPostProcess() = default;

std::string FxaaPostProcess::getClassName() const
{
  return "FxaaPostProcess";
}

std::string FxaaPostProcess::_getDefines()
{
  const auto engine = getEngine();
  if (!engine) {
    return "";
  }

  const auto glInfo = engine->getGlInfo();
  if (!glInfo.renderer.empty()
      && StringTools::contains(StringTools::toLowerCase(glInfo.renderer), "mali")) {
    return "#define MALI 1\n";
  }

  return "";
}

FxaaPostProcessPtr FxaaPostProcess::_Parse(const json& /*parsedPostProcess*/,
                                           const CameraPtr& /*targetCamera*/, Scene* /*scene*/,
                                           const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
