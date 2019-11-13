#include <babylon/postprocesses/vr_multiview_to_singleview_post_process.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

VRMultiviewToSingleviewPostProcess::VRMultiviewToSingleviewPostProcess(
  const std::string& iName, const CameraPtr& camera, float scaleFactor)
    : PostProcess{iName,
                  "vrMultiviewToSingleview",
                  {"imageIndex"},
                  {"multiviewSampler"},
                  scaleFactor,
                  camera,
                  TextureConstants::BILINEAR_SAMPLINGMODE}
{
  onSizeChangedObservable.add([](PostProcess*, EventState&) {});

  onApplyObservable.add([&](Effect* effect, EventState&) -> void {
    if (camera->_scene->activeCamera()
        && camera->_scene->activeCamera()->isLeftCamera()) {
      effect->setInt("imageIndex", 0);
    }
    else {
      effect->setInt("imageIndex", 1);
    }
    // effect->setTexture("multiviewSampler", camera->_multiviewTexture);
  });
}

VRMultiviewToSingleviewPostProcess::~VRMultiviewToSingleviewPostProcess() = default;

} // end of namespace BABYLON
