#ifndef BABYLON_BONES_IK_CONTROLLER_H
#define BABYLON_BONES_IK_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoneIKController {

public:
  BoneIKController(AbstractMesh* mesh, Bone* bone, AbstractMesh* target,
                   AbstractMesh* poleTarget, float poleAngle = 0.f);
  ~BoneIKController();

  float maxAngle() const;
  void setMaxAngle(float value = Math::PI);
  void update();

private:
  void _setMaxAngle(float ang);

public:
  AbstractMesh* target;
  AbstractMesh* poleTarget;
  float poleAngle;
  AbstractMesh* mesh;

private:
  Bone* _bone1;
  Bone* _bone2;
  float _bone1Length;
  float _bone2Length;
  float _maxAngle;
  float _maxReach;

  Vector3 _tmpVec1;
  Vector3 _tmpVec2;
  Vector3 _tmpVec3;
  Vector3 _tmpVec4;
  Vector3 _tmpVec5;

  Matrix _tmpMat1;
  Matrix _tmpMat2;

  bool _rightHandedSystem;

}; // end of class BoneIKController

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_IK_CONTROLLER_H
