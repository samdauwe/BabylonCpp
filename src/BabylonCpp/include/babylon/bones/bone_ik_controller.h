#ifndef BABYLON_BONES_IK_CONTROLLER_H
#define BABYLON_BONES_IK_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BoneIKControllerOptions {
  AbstractMesh* targetMesh                = nullptr;
  AbstractMesh* poleTargetMesh            = nullptr;
  Bone* poleTargetBone                    = nullptr;
  Nullable<Vector3> poleTargetLocalOffset = nullptr;
  Nullable<float> poleAngle               = nullptr;
  Nullable<Vector3> bendAxis              = nullptr;
  Nullable<float> maxAngle                = nullptr;
  Nullable<float> slerpAmount             = nullptr;
}; // end of struct BoneIKControllerOptions

class BABYLON_SHARED_EXPORT BoneIKController {

public:
  BoneIKController(AbstractMesh* mesh, Bone* bone,
                   const Nullable<BoneIKControllerOptions>& options);
  ~BoneIKController();

  void update();

private:
  float get_maxAngle() const;
  void set_maxAngle(float value = Math::PI);
  void _setMaxAngle(float ang = Math::PI);

public:
  AbstractMesh* targetMesh;
  AbstractMesh* poleTargetMesh;
  Bone* poleTargetBone;
  Vector3 targetPosition;
  Vector3 poleTargetPosition;
  Vector3 poleTargetLocalOffset;
  float poleAngle;
  AbstractMesh* mesh;
  float slerpAmount;

  Property<BoneIKController, float> maxAngle;

private:
  static array_t<Vector3, 6> _tmpVecs;
  static Quaternion _tmpQuat;
  static array_t<Matrix, 2> _tmpMats;

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
