#ifndef BABYLON_CAMERAS_VR_POSE_CONTROLLED_H
#define BABYLON_CAMERAS_VR_POSE_CONTROLLED_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/vr/device_pose.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PoseControlled {
  virtual ~PoseControlled() = default;
  Vector3 position;
  Quaternion rotationQuaternion;
  Vector3 devicePosition;
  Quaternion deviceRotationQuaternion;
  DevicePose rawPose;
  float deviceScaleFactor;
  virtual void updateFromDevice(const DevicePose& poseData) = 0;
}; // end of struct PoseControlled

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_VR_POSE_CONTROLLED_H
