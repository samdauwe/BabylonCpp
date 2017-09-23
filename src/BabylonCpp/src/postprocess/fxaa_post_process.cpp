#include <babylon/postprocess/fxaa_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

FxaaPostProcess::FxaaPostProcess(const string_t& iName, float ratio,
                                 Camera* camera, unsigned int samplingMode,
                                 Engine* engine, bool reusable,
                                 unsigned int textureType)
    : PostProcess{iName,        "fxaa", {"texelSize"}, {}, ratio,       camera,
                  samplingMode, engine, reusable,      "", textureType, "fxaa"}
{
  onApplyObservable.add([this](Effect* effect, EventState&) {
    auto _texelSize = texelSize();
    effect->setFloat2("texelSize", _texelSize.x, _texelSize.y);
  });
}

FxaaPostProcess::~FxaaPostProcess()
{
}

} // end of namespace BABYLON
