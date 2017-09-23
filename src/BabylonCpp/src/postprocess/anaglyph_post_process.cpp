#include <babylon/postprocess/anaglyph_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

AnaglyphPostProcess::AnaglyphPostProcess(const string_t& iName, float ratio,
                                         vector_t<Camera*> rigCameras,
                                         unsigned int samplingMode,
                                         Engine* engine, bool reusable)
    : PostProcess{iName,         "anaglyph",   {},     {"leftSampler"}, ratio,
                  rigCameras[1], samplingMode, engine, reusable}
{
  _passedProcess = rigCameras[0]->_rigPostProcess;

  onApplyObservable.add([&](Effect* effect, EventState&) {
    effect->setTextureFromPostProcess("leftSampler", _passedProcess);
  });
}

AnaglyphPostProcess::~AnaglyphPostProcess()
{
}

} // end of namespace BABYLON
