#ifndef BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H
#define BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/cameras/vr/pose_controlled.h>
#include <babylon/gamepad/generic_pad.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

/**
 * @brief Defines the types of pose enabled controllers that are supported.
 */
class BABYLON_SHARED_EXPORT PoseEnabledController : public Gamepad,
                                                    public PoseControlled {

public:
  enum class PoseEnabledControllerType {
    /**
     * HTC Vive
     */
    VIVE,
    /**
     * Oculus Rift
     */
    OCULUS,
    /**
     * Windows mixed reality
     */
    WINDOWS,
    /**
     * Samsung gear VR
     */
    GEAR_VR,
    /**
     * Google Daydream
     */
    DAYDREAM,
    /**
     * Generic
     */
    GENERIC
  }; // end of enum class PoseEnabledControllerType

public:
  /**
   * @brief Creates a new PoseEnabledController from a gamepad.
   * @param browserGamepad the gamepad that the PoseEnabledController should be
   * created from
   */
  PoseEnabledController(const shared_ptr_t<IBrowserGamepad>& browserGamepad);
  ~PoseEnabledController() override;

  /**
   * @brief Updates the state of the pose enbaled controller and mesh based on
   * the current position and rotation of the controller.
   */
  void update() override;

  /**
   * @brief Updates the state of the pose enbaled controller based on the raw
   * pose data from the device.
   * @param poseData raw pose fromthe device
   */
  void updateFromDevice(const DevicePose& poseData) override;

  /**
   * @brief Attaches a mesh to the controller.
   * @param mesh the mesh to be attached
   */
  void attachToMesh(const AbstractMeshPtr& mesh);

  /**
   * @brief Attaches the controllers mesh to a camera.
   * @param camera the camera the mesh should be attached to
   */
  void attachToPoseControlledCamera(TargetCamera* camera);

  /**
   * @brief Disposes of the controller.
   */
  virtual void dispose() override;

  /**
   * @brief Gets the ray of the controller in the direction the controller is
   * pointing.
   * @param length the length the resulting ray should be
   * @returns a ray in the direction the controller is pointing
   */
  Ray getForwardRay(float length = 100.f);

protected:
  /**
   * @brief Updates only the pose device and mesh without doing any button event
   * checking.
   */
  void _updatePoseAndMesh();

protected:
  /**
   * @brief The mesh that is attached to the controller.
   */
  AbstractMeshPtr& get_mesh();

public:
  /**
   * The type of controller (Eg. Windows mixed reality)
   */
  PoseEnabledControllerType controllerType;

  /**
   * Internal, the mesh attached to the controller
   * Hidden
   */
  AbstractMeshPtr _mesh; // a node that will be attached to this Gamepad

  /**
   * Internal, matrix used to convert room space to babylon space
   * Hidden
   */
  Matrix _deviceToWorld;

  /**
   * Node to be used when casting a ray from the controller
   * Hidden
   */
  AbstractMesh* _pointingPoseNode;

  /**
   * The mesh that is attached to the controller.
   */
  ReadOnlyProperty<PoseEnabledController, AbstractMeshPtr> mesh;

  /**
   * Hidden
   */
  Observable<AbstractMesh> _meshAttachedObservable;

  /**
   * Name of the child mesh that can be used to cast a ray from the controller
   */
  static constexpr const char* POINTING_POSE = "POINTING_POSE";

protected:
  Vector3 _calculatedPosition;

private:
  // Represents device position and rotation in room space. Should only be used
  // to help calculate babylon space values
  Vector3 _deviceRoomPosition;
  Quaternion _deviceRoomRotationQuaternion;

  Quaternion _calculatedRotation;

  TargetCamera* _poseControlledCamera;
  Quaternion _leftHandSystemQuaternion;
  Matrix _workingMatrix;

}; // end of struct PoseEnabledController

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_CONTROLLERS_POSE_ENABLED_CONTROLLER_H
