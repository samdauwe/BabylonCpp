#include <babylon/postprocess/fxaa_post_process.h>

#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

FxaaPostProcess::FxaaPostProcess(const std::string& iName, float ratio,
                                 const CameraPtr& camera,
                                 unsigned int samplingMode, Engine* engine,
                                 bool reusable, unsigned int textureType)
    : PostProcess{iName,        "fxaa", {"texelSize"}, {}, ratio,       camera,
                  samplingMode, engine, reusable,      "", textureType, "fxaa",
                  {},           true}
{
  const auto defines = _getDefines();
  updateEffect(defines);

  onApplyObservable.add([this](Effect* effect, EventState&) {
    auto _texelSize = texelSize();
    effect->setFloat2("texelSize", _texelSize.x, _texelSize.y);
  });
}

FxaaPostProcess::~FxaaPostProcess()
{
}

std::string FxaaPostProcess::_getDefines()
{
  auto engine = getEngine();
  if (!engine) {
    return "";
  }

  auto glInfo = engine->getGlInfo();
  if (!glInfo.renderer.empty()
      && String::contains(String::toLowerCase(glInfo.renderer), "mali")) {
    return "#define MALI 1\n";
  }

  return "";
}

} // end of namespace BABYLON
