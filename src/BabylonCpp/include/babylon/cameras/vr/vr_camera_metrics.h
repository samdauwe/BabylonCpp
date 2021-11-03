#ifndef BABYLON_CAMERAS_VR_VR_CAMERA_METRICS_H
#define BABYLON_CAMERAS_VR_VR_CAMERA_METRICS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Matrix;

/**
 * @brief This represents all the required metrics to create a VR camera.
 * @see http://doc.babylonjs.com/babylon101/cameras#device-orientation-camera
 */
class BABYLON_SHARED_EXPORT VRCameraMetrics {

public:
  /**
   * @brief Instantiate the metrics object.
   */
  VRCameraMetrics();
  VRCameraMetrics(VRCameraMetrics& other);
  VRCameraMetrics(VRCameraMetrics&& other);
  VRCameraMetrics& operator=(const VRCameraMetrics& other);
  VRCameraMetrics& operator=(VRCameraMetrics&& other);
  ~VRCameraMetrics(); // = default

  /**
   * @brief Gets the rendering aspect ratio based on the provided resolutions.
   */
  float aspectRatio() const;

  /**
   * @brief Gets the aspect ratio based on the FOV, scale factors, and real
   * screen sizes.
   */
  float aspectRatioFov() const;

  /**
   * @brief Hidden
   */
  Matrix leftHMatrix() const;

  /**
   * @brief Hidden
   */
  Matrix rightHMatrix() const;

  /**
   * @brief Hidden
   */
  Matrix leftPreViewMatrix() const;

  /**
   * @brief Hidden
   */
  Matrix rightPreViewMatrix() const;

  /**
   * @brief Get the default VRMetrics based on the most generic setup.
   * @returns the default vr metrics
   */
  static VRCameraMetrics GetDefault();

public:
  /**
   * Define the horizontal resolution off the screen.
   */
  unsigned int hResolution;

  /**
   * Define the vertical resolution off the screen.
   */
  unsigned int vResolution;

  /**
   * Define the horizontal screen size.
   */
  float hScreenSize;

  /**
   * Define the vertical screen size.
   */
  float vScreenSize;

  /**
   * Define the vertical screen center position.
   */
  float vScreenCenter;

  /**
   * Define the distance of the eyes to the screen.
   */
  float eyeToScreenDistance;

  /**
   * Define the distance between both lenses
   */
  float lensSeparationDistance;

  /**
   * Define the distance between both lenses
   */
  float interpupillaryDistance;

  /**
   * Define the distortion factor of the VR postprocess.
   * Please, touch with care.
   */
  Float32Array distortionK;

  /**
   * Define the chromatic aberration correction factors for the VR post process.
   */
  Float32Array chromaAbCorrection;

  /**
   * Define the scale factor of the post process.
   * The smaller the better but the slower.
   */
  float postProcessScaleFactor;

  /**
   * Define an offset for the lens center.
   */
  float lensCenterOffset;

  /**
   * Define if the current vr camera should compensate the distortion of the
   * lense or not.
   */
  bool compensateDistortion;

  /**
   * Defines if multiview should be enabled when rendering (Default: false)
   */
  bool multiviewEnabled;

}; // end of class VRCameraMetrics

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VR_VR_CAMERA_METRICS_H
