#include <babylon/postprocess/stereoscopic_interlace_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

StereoscopicInterlacePostProcess::StereoscopicInterlacePostProcess(
  const std::string& iName, std::vector<Camera*> rigCameras,
  bool isStereoscopicHoriz, unsigned int samplingMode, Engine* engine,
  bool reusable)
    : PostProcess(iName, "stereoscopicInterlace", {"stepSize"}, {"camASampler"},
                  1.f, rigCameras[1], samplingMode, engine, reusable,
                  isStereoscopicHoriz ? "#define IS_STEREOSCOPIC_HORIZ 1" : "")
{
  _passedProcess = rigCameras[0]->_rigPostProcess;
  _stepSize      = Vector2(1.f / static_cast<float>(width),
                      1.f / static_cast<float>(height));

  onSizeChangedObservable.add([&]() {
    _stepSize = Vector2(1.f / static_cast<float>(width),
                        1.f / static_cast<float>(height));
  });
  onApplyObservable.add([&](Effect* effect) {
    effect->setTextureFromPostProcess("camASampler", _passedProcess);
    effect->setFloat2("stepSize", _stepSize.x, _stepSize.y);
  });
}

StereoscopicInterlacePostProcess::~StereoscopicInterlacePostProcess()
{
}

} // end of namespace BABYLON
