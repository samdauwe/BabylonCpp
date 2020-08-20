#include <babylon/postprocesses/stereoscopic_interlace_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

StereoscopicInterlacePostProcess::StereoscopicInterlacePostProcess(
  const std::string& iName, const std::vector<CameraPtr>& rigCameras, bool isStereoscopicHoriz,
  const std::optional<unsigned int>& samplingMode, Engine* engine, bool reusable)
    : PostProcess(iName, "stereoscopicInterlace", {"stepSize"}, {"camASampler"}, 1.f, rigCameras[1],
                  samplingMode, engine, reusable,
                  isStereoscopicHoriz ? "#define IS_STEREOSCOPIC_HORIZ 1" : "")
{
  _passedProcess = rigCameras[0]->_rigPostProcess;
  _stepSize      = Vector2(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));

  onSizeChangedObservable.add([this](PostProcess*, EventState&) {
    _stepSize = Vector2(1.f / static_cast<float>(width), 1.f / static_cast<float>(height));
  });
  onApplyObservable.add([this](Effect* effect, EventState&) {
    effect->setTextureFromPostProcess("camASampler", _passedProcess);
    effect->setFloat2("stepSize", _stepSize.x, _stepSize.y);
  });
}

StereoscopicInterlacePostProcess::~StereoscopicInterlacePostProcess() = default;

std::string StereoscopicInterlacePostProcess::getClassName() const
{
  return "StereoscopicInterlacePostProcess";
}

} // end of namespace BABYLON
