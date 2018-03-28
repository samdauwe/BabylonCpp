#ifndef BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/vr/pose_controlled.h>
#include <babylon/gamepad/generic_pad.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PoseEnabledController : public Gamepad,
                                                    public PoseControlled {

public:
  enum class PoseEnabledControllerType {
    VIVE,
    OCULUS,
    WINDOWS,
    GEAR_VR,
    /**
     * Google Daydream
     */
    DAYDREAM,
    GENERIC
  }; // end of enum class PoseEnabledControllerType

public:
  PoseEnabledController(const shared_ptr_t<IBrowserGamepad>& browserGamepad);
  ~PoseEnabledController() override;

  void update() override;
  void updateFromDevice(const DevicePose& poseData) override;
  void attachToMesh(AbstractMesh* mesh);
  void attachToPoseControlledCamera(TargetCamera* camera);
  virtual void dispose() override;
  AbstractMesh* mesh();
  Ray getForwardRay(float length = 100.f);

public:
  PoseEnabledControllerType controllerType;
  AbstractMesh* _mesh; // a node that will be attached to this Gamepad
  Matrix _deviceToWorld;

  /**
   * Node to be used when casting a ray from the controller
   */
  AbstractMesh* _pointingPoseNode;
  /**
   * Name of the child mesh that can be used to cast a ray from the controller
   */
  static constexpr const char* POINTING_POSE = "POINTING_POSE";

private:
  // Represents device position and rotation in room space. Should only be used
  // to help calculate babylon space values
  Vector3 _deviceRoomPosition;
  Quaternion _deviceRoomRotationQuaternion;

  Vector3 _calculatedPosition;
  Quaternion _calculatedRotation;

  TargetCamera* _poseControlledCamera;
  Quaternion _leftHandSystemQuaternion;
  Matrix _workingMatrix;

}; // end of struct PoseEnabledController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H
