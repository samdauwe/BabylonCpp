#ifndef BABYLON_POSTPROCESS_VR_DISTORTION_CORRECTION_POST_PROCESS_H
#define BABYLON_POSTPROCESS_VR_DISTORTION_CORRECTION_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/vr/vr_camera_metrics.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT VRDistortionCorrectionPostProcess
    : public PostProcess {

public:
  VRDistortionCorrectionPostProcess(const string_t& name,
                                    const CameraPtr& camera, bool isRightEye,
                                    const VRCameraMetrics& vrMetrics);
  ~VRDistortionCorrectionPostProcess();

private:
  bool _isRightEye;
  Float32Array _distortionFactors;
  float _postProcessScaleFactor;
  float _lensCenterOffset;
  Vector2 _scaleIn;
  Vector2 _scaleFactor;
  Vector2 _lensCenter;

}; // end of class VRDistortionCorrectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_VR_DISTORTION_CORRECTION_POST_PROCESS_H
