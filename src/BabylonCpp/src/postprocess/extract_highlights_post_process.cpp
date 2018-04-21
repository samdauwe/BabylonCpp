#include <babylon/postprocess/extract_highlights_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

ExtractHighlightsPostProcess::ExtractHighlightsPostProcess(
  const string_t& iName, const Variant<float, PostProcessOptions>& options,
  Camera* camera, unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType, bool blockCompilation)
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
    effect->setFloat("threshold", ::std::pow(threshold, Math::ToGammaSpace));
    effect->setFloat("exposure", _exposure);
  });
}

ExtractHighlightsPostProcess::~ExtractHighlightsPostProcess()
{
}

} // end of namespace BABYLON
