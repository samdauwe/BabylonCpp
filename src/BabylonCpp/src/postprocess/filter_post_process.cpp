#include <babylon/postprocess/filter_post_process.h>

#include <babylon/core/variant.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

FilterPostProcess::FilterPostProcess(const string_t& iName,
                                     const Matrix& _kernelMatrix, float ratio,
                                     const CameraPtr& camera,
                                     unsigned int samplingMode, Engine* engine,
                                     bool reusable)
    : PostProcess{iName,
                  "filter",
                  {"kernelMatrix"},
                  {},
                  ToVariant<float, PostProcessOptions>(ratio),
                  camera,
                  samplingMode,
                  engine,
                  reusable}
    , kernelMatrix{_kernelMatrix}
{
  setOnApply([&](Effect* effect, EventState&) {
    effect->setMatrix("kernelMatrix", kernelMatrix);
  });
}

FilterPostProcess::~FilterPostProcess()
{
}

} // end of namespace BABYLON
