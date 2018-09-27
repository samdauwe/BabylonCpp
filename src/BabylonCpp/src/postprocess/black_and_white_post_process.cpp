#include <babylon/postprocess/black_and_white_post_process.h>

#include <babylon/core/variant.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

BlackAndWhitePostProcess::BlackAndWhitePostProcess(
  const std::string& iName, float ratio, const CameraPtr& camera,
  unsigned int samplingMode, Engine* engine, bool reusable)
    : PostProcess{iName,
                  "blackAndWhite",
                  {"degree"},
                  {},
                  ToVariant<float, PostProcessOptions>(ratio),
                  camera,
                  samplingMode,
                  engine,
                  reusable}
    , degree{1.f}
{
  onApplyObservable.add([this](Effect* effect, EventState&) {
    effect->setFloat("degree", degree);
  });
}

BlackAndWhitePostProcess::~BlackAndWhitePostProcess()
{
}

} // end of namespace BABYLON
