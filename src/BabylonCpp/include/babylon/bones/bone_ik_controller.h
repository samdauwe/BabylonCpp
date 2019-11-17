#ifndef BABYLON_BONES_IK_CONTROLLER_H
#define BABYLON_BONES_IK_CONTROLLER_H

#include <babylon/babylon_api.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class AbstractMesh;
class Bone;

struct BoneIKControllerOptions {
  AbstractMesh* targetMesh                     = nullptr;
  AbstractMesh* poleTargetMesh                 = nullptr;
  Bone* poleTargetBone                         = nullptr;
  std::optional<Vector3> poleTargetLocalOffset = std::nullopt;
  std::optional<float> poleAngle               = std::nullopt;
  std::optional<Vector3> bendAxis              = std::nullopt;
  std::optional<float> maxAngle                = std::nullopt;
  std::optional<float> slerpAmount             = std::nullopt;
}; // end of struct BoneIKControllerOptions

/**
 * @brief Class used to apply inverse kinematics to bones.
 * @see
 * http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons#boneikcontroller
 */
class BABYLON_SHARED_EXPORT BoneIKController {

public:
  /**
   * @brief Creates a new BoneIKController.
   * @param mesh defines the mesh to control
   * @param bone defines the bone to control
   * @param options defines options to set up the controller
   */
  BoneIKController(AbstractMesh* mesh, Bone* bone,
                   const std::optional<BoneIKControllerOptions>& options);
  ~BoneIKController(); // = default

  /**
   * @brief Force the controller to update the bones.
   */
  void update();

protected:
  /**
   * @brief Gets maximum allowed angle.
   */
  [[nodiscard]] float get_maxAngle() const;

  /**
   * @brief Sets maximum allowed angle.
   */
  void set_maxAngle(float value = Math::PI);

  /**
   * @brief Sets maximum allowed angle.
   */
  void _setMaxAngle(float ang = Math::PI);

public:
  /**
   * Gets or sets the target mesh
   */
  AbstractMesh* targetMesh;

  /**
   * Gets or sets the mesh used as pole
   */
  AbstractMesh* poleTargetMesh;

  /**
   * Gets or sets the bone used as pole
   */
  Bone* poleTargetBone;

  /**
   * Gets or sets the target position
   */
  Vector3 targetPosition;

  /**
   * Gets or sets the pole target position
   */
  Vector3 poleTargetPosition;

  /**
   * Gets or sets the pole target local offset
   */
  Vector3 poleTargetLocalOffset;

  /**
   * Gets or sets the pole angle
   */
  float poleAngle;

  /**
   * Gets or sets the mesh associated with the controller
   */
  AbstractMesh* mesh;

  /**
   * The amount to slerp (spherical linear interpolation) to the target.  Set
   * this to a value between 0 and 1 (a value of 1 disables slerp)
   */
  float slerpAmount;

  /**
   * Gets or sets maximum allowed angle
   */
  Property<BoneIKController, float> maxAngle;

private:
  static std::array<Vector3, 6> _tmpVecs;
  static Quaternion _tmpQuat;
  static std::array<Matrix, 2> _tmpMats;

private:
  Quaternion _bone1Quat;
  Matrix _bone1Mat;
  float _bone2Ang;
  Bone* _bone1;
  Bone* _bone2;
  float _bone1Length;
  float _bone2Length;
  float _maxAngle;
  float _maxReach;
  bool _rightHandedSystem;
  Vector3 _bendAxis;
  bool _slerping;
  float _adjustRoll;

}; // end of class BoneIKController

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_IK_CONTROLLER_H
