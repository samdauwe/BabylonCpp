#ifndef BABYLON_CAMERAS_VR_VR_CAMERA_METRICS_H
#define BABYLON_CAMERAS_VR_VR_CAMERA_METRICS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT VRCameraMetrics {

public:
  VRCameraMetrics();
  VRCameraMetrics(VRCameraMetrics& other);
  VRCameraMetrics(VRCameraMetrics&& other);
  VRCameraMetrics& operator=(const VRCameraMetrics& other);
  VRCameraMetrics& operator=(VRCameraMetrics&& other);
  ~VRCameraMetrics();

  float aspectRatio() const;
  float aspectRatioFov() const;
  Matrix leftHMatrix() const;
  Matrix rightHMatrix() const;
  Matrix leftPreViewMatrix() const;
  Matrix rightPreViewMatrix() const;

  static VRCameraMetrics GetDefault();

public:
  unsigned int hResolution;
  unsigned int vResolution;
  float hScreenSize;
  float vScreenSize;
  float vScreenCenter;
  float eyeToScreenDistance;
  float lensSeparationDistance;
  float interpupillaryDistance;
  Float32Array distortionK;
  Float32Array chromaAbCorrection;
  float postProcessScaleFactor;
  float lensCenterOffset;
  bool compensateDistortion;

}; // end of class FreeCameraMouseInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VR_VR_CAMERA_METRICS_H
