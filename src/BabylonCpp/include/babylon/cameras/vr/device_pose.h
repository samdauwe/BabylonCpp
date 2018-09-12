#ifndef BABYLON_CAMERAS_VR_DEVICE_POSE_H
#define BABYLON_CAMERAS_VR_DEVICE_POSE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT DevicePose {
  Float32Array position;
  Float32Array linearVelocity;
  Float32Array linearAcceleration;

  Float32Array orientation;
  Float32Array angularVelocity;
  Float32Array angularAcceleration;
}; // end of struct DevicePose

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VR_DEVICE_POSE_H
