#ifndef BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/vr/pose_controlled.h>
#include <babylon/gamepad/generic_pad.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PoseEnabledController : public Gamepad,
                                                    public PoseControlled {

public:
  enum class PoseEnabledControllerType {
    VIVE,
    OCULUS,
    WINDOWS,
    GENERIC
  }; // end of enum class PoseEnabledControllerType

public:
  PoseEnabledController(const std::shared_ptr<IBrowserGamepad>& vrGamepad);
  ~PoseEnabledController();

  void update() override;
  void updateFromDevice(const DevicePose& poseData) override;
  void attachToMesh(AbstractMesh* mesh);
  void attachToPoseControlledCamera(TargetCamera* camera);
  void dispose() override;
  AbstractMesh* mesh();
  Ray getForwardRay(float length);

public:
  Vector3 devicePosition;
  Quaternion deviceRotationQuaternion;
  float deviceScaleFactor;

  Vector3 position;
  Quaternion rotationQuaternion;
  PoseEnabledControllerType controllerType;

  DevicePose rawPose;  // GamepadPose;
  AbstractMesh* _mesh; // a node that will be attached to this Gamepad

private:
  Vector3 _calculatedPosition;
  Quaternion _calculatedRotation;

  TargetCamera* _poseControlledCamera;
  Quaternion _leftHandSystemQuaternion;

}; // end of struct PoseEnabledController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H
