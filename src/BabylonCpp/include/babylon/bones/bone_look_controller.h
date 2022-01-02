#ifndef BABYLON_BONES_LOOK_CONTROLLER_H
#define BABYLON_BONES_LOOK_CONTROLLER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>
#include <babylon/maths/axis.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class AbstractMesh;
class Bone;

struct BABYLON_SHARED_EXPORT BoneLookControllerOptions {
  std::optional<float> maxYaw      = std::nullopt;
  std::optional<float> minYaw      = std::nullopt;
  std::optional<float> maxPitch    = std::nullopt;
  std::optional<float> minPitch    = std::nullopt;
  std::optional<float> slerpAmount = std::nullopt;
  std::optional<Vector3> upAxis    = std::nullopt;
  std::optional<Space> upAxisSpace = std::nullopt;
  std::optional<Vector3> yawAxis   = std::nullopt;
  std::optional<Vector3> pitchAxis = std::nullopt;
  std::optional<float> adjustYaw   = std::nullopt;
  std::optional<float> adjustPitch = std::nullopt;
  std::optional<float> adjustRoll  = std::nullopt;
}; // end of struct BoneLookControllerOptions

/**
 * @brief Class used to make a bone look toward a point in space.
 * @see https://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons#bonelookcontroller
 */
class BABYLON_SHARED_EXPORT BoneLookController {

public:
  /**
   * @brief Create a BoneLookController.
   * @param mesh the mesh that the bone belongs to
   * @param bone the bone that will be looking to the target
   * @param target the target Vector3 to look at
   * @param options optional settings:
   * * maxYaw: the maximum angle the bone will yaw to
   * * minYaw: the minimum angle the bone will yaw to
   * * maxPitch: the maximum angle the bone will pitch to
   * * minPitch: the minimum angle the bone will yaw to
   * * slerpAmount: set the between 0 and 1 to make the bone slerp to the target.
   * * upAxis: the up axis of the coordinate system
   * * upAxisSpace: the space that the up axis is in - Space.BONE, Space.LOCAL (default), or
   *Space.WORLD.
   * * yawAxis: set yawAxis if the bone does not yaw on the y axis
   * * pitchAxis: set pitchAxis if the bone does not pitch on the x axis
   * * adjustYaw: used to make an adjustment to the yaw of the bone
   * * adjustPitch: used to make an adjustment to the pitch of the bone
   * * adjustRoll: used to make an adjustment to the roll of the bone
   **/
  BoneLookController(AbstractMesh* mesh, Bone* bone, const Vector3& target,
                     const std::optional<BoneLookControllerOptions>& options = std::nullopt);
  ~BoneLookController(); // = default

  /**
   * @brief Update the bone to look at the target.  This should be called before
   * the scene is rendered (use scene.registerBeforeRender()).
   */
  void update();

protected:
  /**
   * @brief Gets the minimum yaw angle that the bone can look to.
   */
  [[nodiscard]] float get_minYaw() const;

  /**
   * @brief Sets the minimum yaw angle that the bone can look to.
   */
  void set_minYaw(float value);

  /**
   * @brief Gets the maximum yaw angle that the bone can look to.
   */
  [[nodiscard]] float get_maxYaw() const;

  /**
   * @brief Sets the maximum yaw angle that the bone can look to.
   */
  void set_maxYaw(float value);

  /**
   * @brief Gets the minimum pitch angle that the bone can look to.
   */
  [[nodiscard]] float get_minPitch() const;

  /**
   * @brief Sets the minimum pitch angle that the bone can look to.
   */
  void set_minPitch(float value);

  /**
   * @brief Gets the maximum pitch angle that the bone can look to.
   */
  [[nodiscard]] float get_maxPitch() const;

  /**
   * @brief Sets the maximum pitch angle that the bone can look to.
   */
  void set_maxPitch(float value);

private:
  [[nodiscard]] float _getAngleDiff(float ang1, float ang2) const;
  [[nodiscard]] float _getAngleBetween(float ang1, float ang2) const;
  [[nodiscard]] bool _isAngleBetween(float ang, float ang1, float ang2) const;

public:
  /**
   * The target Vector3 that the bone will look at
   */
  Vector3 target;

  /**
   * The mesh that the bone is attached to
   */
  AbstractMesh* mesh;

  /**
   * The bone that will be looking to the target
   */
  Bone* bone;

  /**
   * The up axis of the coordinate system that is used when the bone is rotated
   */
  Vector3 upAxis;

  /**
   * The space that the up axis is in - Space.BONE, Space.LOCAL (default), or Space.WORLD
   */
  Space upAxisSpace;

  /**
   * Used to make an adjustment to the yaw of the bone
   */
  float adjustYaw;

  /**
   * Used to make an adjustment to the pitch of the bone
   */
  float adjustPitch;

  /**
   * Used to make an adjustment to the roll of the bone
   */
  float adjustRoll;

  /**
   * The amount to slerp (spherical linear interpolation) to the target. Set this to a value
   * between 0 and 1 (a value of 1 disables slerp)
   */
  float slerpAmount;

  /**
   * Gets or sets the minimum yaw angle that the bone can look to
   */
  Property<BoneLookController, float> minYaw;

  /**
   * Gets or sets the maximum yaw angle that the bone can look to
   */
  Property<BoneLookController, float> maxYaw;

  /**
   * Gets or sets the minimum pitch angle that the bone can look to
   */
  Property<BoneLookController, float> minPitch;

  /**
   * Gets or sets the maximum pitch angle that the bone can look to
   */
  Property<BoneLookController, float> maxPitch;

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
  std::optional<Matrix> _transformYawPitch;
  Matrix _transformYawPitchInv;
  bool _firstFrameSkipped;
  float _yawRange;
  Vector3 _fowardAxis;

}; // end of class BoneLookController

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_LOOK_CONTROLLER_H
