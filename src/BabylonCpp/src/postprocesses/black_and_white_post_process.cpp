#include <babylon/postprocesses/black_and_white_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

BlackAndWhitePostProcess::BlackAndWhitePostProcess(const std::string& iName, float ratio,
                                                   const CameraPtr& camera,
                                                   const std::optional<unsigned int>& samplingMode,
                                                   Engine* engine, bool reusable)
    : PostProcess{iName,  "blackAndWhite", {"degree"}, {},      ratio,
                  camera, samplingMode,    engine,     reusable}
    , degree{1.f}
{
  onApplyObservable.add(
    [this](Effect* effect, EventState&) { effect->setFloat("degree", degree); });
}

BlackAndWhitePostProcess::~BlackAndWhitePostProcess() = default;

} // end of namespace BABYLON
