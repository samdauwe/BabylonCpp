#ifndef BABYLON_BONES_LOOK_CONTROLLER_H
#define BABYLON_BONES_LOOK_CONTROLLER_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoneLookController {

public:
  BoneLookController(AbstractMesh* mesh, Bone* bone, const Vector3& target,
                     float adjustYaw = 0.f, float adjustPitch = 0.f,
                     float adjustRoll = 0.f);
  ~BoneLookController();

  void update();

public:
  Vector3 target;
  AbstractMesh* mesh;
  Bone* bone;
  Vector3 upAxis;

  float adjustYaw;
  float adjustPitch;
  float adjustRoll;

private:
  Vector3 _tmpVec1;
  Vector3 _tmpVec2;
  Vector3 _tmpVec3;
  Vector3 _tmpVec4;

  Matrix _tmpMat1;
  Matrix _tmpMat2;

}; // end of class BoneLookController

} // end of namespace BABYLON

#endif // end of BABYLON_BONES_LOOK_CONTROLLER_H
