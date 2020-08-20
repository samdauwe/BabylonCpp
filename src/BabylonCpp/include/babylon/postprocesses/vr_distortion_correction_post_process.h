#ifndef BABYLON_POSTPROCESSES_VR_DISTORTION_CORRECTION_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_VR_DISTORTION_CORRECTION_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/vr/vr_camera_metrics.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

/**
 * @brief VRDistortionCorrectionPostProcess used for mobile VR.
 */
class BABYLON_SHARED_EXPORT VRDistortionCorrectionPostProcess : public PostProcess {

public:
  /**
   * @brief Initializes the VRDistortionCorrectionPostProcess.
   * @param name The name of the effect.
   * @param camera The camera to apply the render pass to.
   * @param isRightEye If this is for the right eye distortion
   * @param vrMetrics All the required metrics for the VR camera
   */
  VRDistortionCorrectionPostProcess(const std::string& name, const CameraPtr& camera,
                                    bool isRightEye, const VRCameraMetrics& vrMetrics);
  ~VRDistortionCorrectionPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "VRDistortionCorrectionPostProcess" string
   */
  std::string getClassName() const override;

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

#endif // end of BABYLON_POSTPROCESSES_VR_DISTORTION_CORRECTION_POST_PROCESS_H
