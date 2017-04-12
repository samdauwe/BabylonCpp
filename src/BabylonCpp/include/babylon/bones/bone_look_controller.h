#ifndef BABYLON_BONES_LOOK_CONTROLLER_H
#define BABYLON_BONES_LOOK_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/math/axis.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BoneLookControllerOptions {
  Nullable<float> maxYaw;
  Nullable<float> minYaw;
  Nullable<float> maxPitch;
  Nullable<float> minPitch;
  Nullable<float> slerpAmount;
  Nullable<Vector3> upAxis;
  Nullable<Space> upAxisSpace;
  Nullable<Vector3> yawAxis;
  Nullable<Vector3> pitchAxis;
  Nullable<float> adjustYaw;
  Nullable<float> adjustPitch;
  Nullable<float> adjustRoll;
}; // end of struct BoneLookControllerOptions

class BABYLON_SHARED_EXPORT BoneLookController {

public:
  /**
   * @brief Create a BoneLookController
   * @param mesh the mesh that the bone belongs to
   * @param bone the bone that will be looking to the target
   * @param target the target Vector3 to look at
   * @param settings optional settings:
   * - maxYaw: the maximum angle the bone will yaw to
   * - minYaw: the minimum angle the bone will yaw to
   * - maxPitch: the maximum angle the bone will pitch to
   * - minPitch: the minimum angle the bone will yaw to
   * - slerpAmount: set the between 0 and 1 to make the bone slerp to the
   *                target.
   * - upAxis: the up axis of the coordinate system
   * - upAxisSpace: the space that the up axis is in - BABYLON.Space.BONE,
   *                BABYLON.Space.LOCAL (default), or BABYLON.Space.WORLD.
   * - yawAxis: set yawAxis if the bone does not yaw on the y axis
   * - pitchAxis: set pitchAxis if the bone does not pitch on the x axis
   * - adjustYaw: used to make an adjustment to the yaw of the bone
   * - adjustPitch: used to make an adjustment to the pitch of the bone
   * - adjustRoll: used to make an adjustment to the roll of the bone
   **/
  BoneLookController(AbstractMesh* mesh, Bone* bone, const Vector3& target,
                     const Nullable<BoneLookControllerOptions>& options
                     = nullptr);
  ~BoneLookController();

  /**
   * @brief Get the minimum yaw angle that the bone can look to.
   */
  float minYaw() const;

  /**
   * @brief Set the minimum yaw angle that the bone can look to.
   */
  void setMinYaw(float value);

  /**
   * @brief Get the maximum yaw angle that the bone can look to.
   */
  float maxYaw() const;

  /**
   * @brief Set the maximum yaw angle that the bone can look to.
   */
  void setMaxYaw(float value);

  /**
   * @brief Get the minimum pitch angle that the bone can look to.
   */
  float minPitch() const;

  /**
   * @brief Set the minimum pitch angle that the bone can look to.
   */
  void setMinPitch(float value);

  /**
   * @brief Get the maximum pitch angle that the bone can look to.
   */
  float maxPitch() const;

  /**
   * @brief Set the maximum pitch angle that the bone can look to.
   */
  void setMaxPitch(float value);

  /**
   * @brief Update the bone to look at the target. This should be called before
   * the scene is rendered (use scene.registerBeforeRender()).
   */
  void update();

private:
  float _getAngleDiff(float ang1, float ang2) const;
  float _getAngleBetween(float ang1, float ang2) const;
  bool _isAngleBetween(float ang, float ang1, float ang2) const;

public:
  /**
   * The target Vector3 that the bone will look at.
   */
  Vector3 target;

  /**
   * The mesh that the bone is attached to.
   */
  AbstractMesh* mesh;

  /**
   * The bone that will be looking to the target.
   */
  Bone* bone;

  /**
   * The up axis of the coordinate system that is used when the bone is rotated.
   */
  Vector3 upAxis;

  /**
   * The space that the up axis is in - BABYLON.Space.BONE, BABYLON.Space.LOCAL
   * (default), or BABYLON.Space.WORLD.
   */
  Space upAxisSpace;

  /**
   * Used to make an adjustment to the yaw of the bone.
   */
  float adjustYaw;

  /**
   * Used to make an adjustment to the pitch of the bone.
   */
  float adjustPitch;

  /**
   * Used to make an adjustment to the roll of the bone.
   */
  float adjustRoll;

  /**
   * The amount to slerp (spherical linear interpolation) to the target.  Set
   * this to a value between 0 and 1 (a value of 1 disables slerp).
   */
  float slerpAmount;

private:
  static std::array<Vector3, 10> _tmpVecs;
  static Quaternion _tmpQuat;
  static std::array<Matrix, 5> _tmpMats;

private:
  bool _minYawSet;
  float _minYaw;
  bool _maxYawSet;
  float _maxYaw;
  float _minPitch;
  float _maxPitch;
  float _minYawSin;
  float _minYawCos;
  float _maxYawSin;
  float _maxYawCos;
  float _midYawConstraint;
  float _minPitchTan;
  float _maxPitchTan;
  Quaternion _boneQuat;
  bool _slerping;
  Nullable<Matrix> _transformYawPitch;
  Matrix _transformYawPitchInv;
  bool _firstFrameSkipped;
  float _yawRange;
  Vector3 _fowardAxis;

}; // end of class BoneLookController

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_LOOK_CONTROLLER_H
