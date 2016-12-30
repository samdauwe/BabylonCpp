#include <babylon/bones/bone_look_controller.h>

#include <babylon/bones/bone.h>

namespace BABYLON {

BoneLookController::BoneLookController(AbstractMesh* iMesh, Bone* iBone,
                                       const Vector3& iTarget, float iAdjustYaw,
                                       float iAdjustPitch, float iAdjustRoll)
    : target{iTarget}
    , mesh{iMesh}
    , bone{iBone}
    , upAxis{Vector3::Up()}
    , adjustYaw{iAdjustYaw}
    , adjustPitch{iAdjustPitch}
    , adjustRoll{iAdjustRoll}
    , _tmpVec1{Vector3::Zero()}
    , _tmpVec2{Vector3::Zero()}
    , _tmpVec3{Vector3::Zero()}
    , _tmpVec4{Vector3::Zero()}
    , _tmpMat1{Matrix::Identity()}
    , _tmpMat2{Matrix::Identity()}
{
}

BoneLookController::~BoneLookController()
{
}

void BoneLookController::update()
{
  Vector3 _target = target;

  Vector3& bonePos = _tmpVec1;
  Vector3& zaxis   = _tmpVec2;
  Vector3& xaxis   = _tmpVec3;
  Vector3& yaxis   = _tmpVec4;
  Matrix& mat1     = _tmpMat1;
  Matrix& mat2     = _tmpMat2;

  bone->getAbsolutePositionToRef(mesh, bonePos);

  _target.subtractToRef(bonePos, zaxis);
  zaxis.normalize();

  Vector3::CrossToRef(upAxis, zaxis, xaxis);
  xaxis.normalize();

  Vector3::CrossToRef(zaxis, xaxis, yaxis);
  yaxis.normalize();

  Matrix::FromXYZAxesToRef(xaxis, yaxis, zaxis, mat1);

  if (!std_util::almost_equal(adjustYaw, 0.f)
      || !std_util::almost_equal(adjustPitch, 0.f)
      || !std_util::almost_equal(adjustRoll, 0.f)) {
    Matrix::RotationYawPitchRollToRef(adjustYaw, adjustPitch, adjustRoll, mat2);
    mat2.multiplyToRef(mat1, mat1);
  }

  bone->setRotationMatrix(mat1, Space::WORLD, mesh);
}

} // end of namespace BABYLON
