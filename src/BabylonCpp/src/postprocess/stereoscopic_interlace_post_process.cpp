#include <babylon/postprocess/stereoscopic_interlace_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/variant.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

StereoscopicInterlacePostProcess::StereoscopicInterlacePostProcess(
  const string_t& iName, vector_t<Camera*> rigCameras, bool isStereoscopicHoriz,
  unsigned int samplingMode, Engine* engine, bool reusable)
    : PostProcess(iName, "stereoscopicInterlace", {"stepSize"}, {"camASampler"},
                  ToVariant<float, PostProcessOptions>(1.f), rigCameras[1],
                  samplingMode, engine, reusable,
                  isStereoscopicHoriz ? "#define IS_STEREOSCOPIC_HORIZ 1" : "")
{
  _passedProcess = rigCameras[0]->_rigPostProcess;
  _stepSize      = Vector2(1.f / static_cast<float>(width),
                      1.f / static_cast<float>(height));

  onSizeChangedObservable.add([&](PostProcess*, EventState&) {
    _stepSize = Vector2(1.f / static_cast<float>(width),
                        1.f / static_cast<float>(height));
  });
  onApplyObservable.add([&](Effect* effect, EventState&) {
    effect->setTextureFromPostProcess("camASampler", _passedProcess);
    effect->setFloat2("stepSize", _stepSize.x, _stepSize.y);
  });
}

StereoscopicInterlacePostProcess::~StereoscopicInterlacePostProcess()
{
}

} // end of namespace BABYLON
