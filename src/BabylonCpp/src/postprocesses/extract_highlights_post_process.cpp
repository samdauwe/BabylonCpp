#include <babylon/postprocesses/extract_highlights_post_process.h>

#include <cmath>

#include <babylon/materials/effect.h>

namespace BABYLON {

ExtractHighlightsPostProcess::ExtractHighlightsPostProcess(
  const std::string& iName, const std::variant<float, PostProcessOptions>& options,
  const CameraPtr& camera, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable, unsigned int textureType, bool blockCompilation)
    : PostProcess{iName,
                  "extractHighlights",
                  {"threshold", "exposure"},
                  {},
                  options,
                  camera,
                  samplingMode,
                  engine,
                  reusable,
                  "",
                  textureType,
                  "",
                  {},
                  blockCompilation}
    , threshold{0.9f}
    , _exposure{1.f}
    , _inputPostProcess{nullptr}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    if (_inputPostProcess) {
      effect->setTextureFromPostProcess("textureSampler", _inputPostProcess);
    }
    effect->setFloat("threshold", std::pow(threshold, Math::ToGammaSpace));
    effect->setFloat("exposure", _exposure);
  });
}

ExtractHighlightsPostProcess::~ExtractHighlightsPostProcess() = default;

} // end of namespace BABYLON
