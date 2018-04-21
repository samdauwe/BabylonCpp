#include <babylon/postprocess/grain_post_process.h>

#include <babylon/core/random.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

GrainPostProcess::GrainPostProcess(
  const string_t& iName, const Variant<float, PostProcessOptions>& options,
  Camera* camera, unsigned int samplingMode, Engine* engine, bool reusable,
  unsigned int textureType, bool blockCompilation)
    : PostProcess{iName,        "grain",         {"intensity", "animatedSeed"},
                  {},           options,         camera,
                  samplingMode, engine,          reusable,
                  "",           textureType,     "",
                  {},           blockCompilation}
    , intensity{30.f}
    , animated{false}
{
  onApplyObservable.add([&](Effect* effect, EventState& /*es*/) {
    effect->setFloat("intensity", intensity);
    effect->setFloat("animatedSeed", animated ? Math::random() + 1.f : 1.f);
  });
}

GrainPostProcess::~GrainPostProcess()
{
}

} // end of namespace BABYLON
