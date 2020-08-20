#include <babylon/postprocesses/stereoscopic_interlace_post_process_i.h>

#include <babylon/cameras/camera.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

StereoscopicInterlacePostProcessI::StereoscopicInterlacePostProcessI(
  const std::string& iName, const std::vector<CameraPtr>& rigCameras, bool isStereoscopicHoriz,
  bool isStereoscopicInterlaced, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable)
    : PostProcess(iName, "stereoscopicInterlace", {"stepSize"}, {"camASampler"}, 1.f, rigCameras[1],
                  samplingMode, engine, reusable,
                  isStereoscopicInterlaced ?
                    "#define IS_STEREOSCOPIC_INTERLACED 1" :
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

StereoscopicInterlacePostProcessI::~StereoscopicInterlacePostProcessI() = default;

std::string StereoscopicInterlacePostProcessI::getClassName() const
{
  return "StereoscopicInterlacePostProcessI";
}

} // end of namespace BABYLON
