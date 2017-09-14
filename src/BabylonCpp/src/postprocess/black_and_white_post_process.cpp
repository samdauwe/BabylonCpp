#include <babylon/postprocess/black_and_white_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

BlackAndWhitePostProcess::BlackAndWhitePostProcess(const string_t& iName,
                                                   float ratio, Camera* camera,
                                                   unsigned int samplingMode,
                                                   Engine* engine,
                                                   bool reusable)
    : PostProcess{iName,  "blackAndWhite", {"degree"}, {},      ratio,
                  camera, samplingMode,    engine,     reusable}
    , degree{1.f}
{
  onApplyObservable.add(
    [this](Effect* effect) { effect->setFloat("degree", degree); });
}

BlackAndWhitePostProcess::~BlackAndWhitePostProcess()
{
}

} // end of namespace BABYLON
