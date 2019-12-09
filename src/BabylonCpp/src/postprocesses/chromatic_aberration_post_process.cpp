#include <babylon/postprocesses/chromatic_aberration_post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

ChromaticAberrationPostProcess::ChromaticAberrationPostProcess(
  const std::string& iName, int iScreenWidth, int iScreenHeight,
  const std::variant<float, PostProcessOptions>& options, const CameraPtr& camera,
  const std::optional<unsigned int>& samplingMode, Engine* engine, bool reusable,
  unsigned int textureType, bool blockCompilation)
    : PostProcess{iName,
                  "chromaticAberration",
                  {"chromatic_aberration", "screen_width", "screen_height", "direction",
                   "radialIntensity", "centerPosition"},
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
    , aberrationAmount{30.f}
    , radialIntensity{0.f}
    , direction{Vector2{0.707f, 0.707f}}
    , centerPosition{Vector2{0.5f, 0.5f}}
    , screenWidth{static_cast<float>(iScreenWidth)}
    , screenHeight{static_cast<float>(iScreenHeight)}
{
  onApplyObservable.add([&](Effect* effect, EventState&) {
    effect->setFloat("chromatic_aberration", aberrationAmount);
    effect->setFloat("screen_width", screenWidth);
    effect->setFloat("screen_height", screenHeight);
    effect->setFloat("radialIntensity", radialIntensity);
    effect->setFloat2("direction", direction.x, direction.y);
    effect->setFloat2("centerPosition", centerPosition.x, centerPosition.y);
  });
}

ChromaticAberrationPostProcess::~ChromaticAberrationPostProcess() = default;

} // end of namespace BABYLON
