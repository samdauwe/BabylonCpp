#include <babylon/bones/bone_ik_controller.h>

#include <babylon/bones/bone.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

std::array<Vector3, 6> BoneIKController::_tmpVecs{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero()}};

Quaternion BoneIKController::_tmpQuat{Quaternion::Identity()};

std::array<Matrix, 2> BoneIKController::_tmpMats{
  {Matrix::Identity(), Matrix::Identity()}};

BoneIKController::BoneIKController(AbstractMesh* iMesh, Bone* bone,
                                   AbstractMesh* iTargetMesh,
                                   AbstractMesh* iPoleTargetMesh,
                                   Bone* iPoleTargetBone,
                                   const Vector3& iPoleTargetLocalOffset,
                                   float iPoleAngle, const Vector3& iBendAxis,
                                   float iMaxAngle, float iSlerpAmount)
    : targetMesh{nullptr}
    , poleTargetMesh{nullptr}
    , targetPosition{Vector3::Zero()}
    , poleTargetPosition{Vector3::Zero()}
    , poleTargetLocalOffset{iPoleTargetLocalOffset}
    , poleAngle{iPoleAngle}
    , mesh{iMesh}
    , slerpAmount{iSlerpAmount}
    , _bone1Quat{Quaternion::Identity()}
    , _bone1Mat{Matrix::Identity()}
    , _bone2Ang{Math::PI}
    , _bone1{bone->getParent()}
    , _bone2{bone}
    , _maxAngle{iMaxAngle}
    , _rightHandedSystem{false}
    , _bendAxis{iBendAxis}
    , _slerping{false}
    , _adjustRoll{0.f}
{
  auto bonePos = bone->getPosition();

  if (bone->getAbsoluteTransform().determinant() > 0.f) {
    _rightHandedSystem = true;
    _bendAxis.x        = 0.f;
    _bendAxis.y        = 0.f;
    _bendAxis.z        = -1.f;

    if (bonePos.x > bonePos.y && bonePos.x > bonePos.z) {
      _adjustRoll = Math::PI * 0.5f;
      _bendAxis.z = 1;
    }
  }

  if (_bone1->length) {

    auto boneScale1 = _bone1->getScale();
    auto boneScale2 = _bone2->getScale();

    _bone1Length = _bone1->length * boneScale1.y * mesh->scaling().y;
    _bone2Length = _bone2->length * boneScale2.y * mesh->scaling().y;
  }
  else if (!_bone1->children.empty() && _bone1->children[0]) {

    mesh->computeWorldMatrix(true);

    auto pos1 = _bone2->children[0]->getAbsolutePosition(iMesh);
    auto pos2 = _bone2->getAbsolutePosition(iMesh);
    auto pos3 = _bone1->getAbsolutePosition(iMesh);

    _bone1Length = Vector3::Distance(pos1, pos2);
    _bone2Length = Vector3::Distance(pos2, pos3);
  }

  _bone1->getRotationMatrixToRef(_bone1Mat, Space::WORLD, mesh);
  setMaxAngle(Math::PI);

  // Options
  {
    if (iTargetMesh) {
      targetMesh = iTargetMesh;
      targetMesh->computeWorldMatrix(true);
    }

    if (iPoleTargetMesh) {
      poleTargetMesh = iPoleTargetMesh;
      poleTargetMesh->computeWorldMatrix(true);
    }
    else if (iPoleTargetBone) {
      poleTargetBone = iPoleTargetBone;
    }
    else if (_bone1->getParent()) {
      poleTargetBone = _bone1->getParent();
    }
  }
}

BoneIKController::~BoneIKController()
{
}

float BoneIKController::maxAngle() const
{
  return _maxAngle;
}

void BoneIKController::setMaxAngle(float value)
{
  _setMaxAngle(value);
}

void BoneIKController::_setMaxAngle(float ang)
{
  if (ang < 0.f) {
    ang = 0.f;
  }

  if (ang > Math::PI) {
    ang = Math::PI;
  }

  _maxAngle = ang;

  const float a = _bone1Length;
  const float b = _bone2Length;

  _maxReach = std::sqrt(a * a + b * b - 2 * a * b * std::cos(ang));
}

void BoneIKController::update()
{
  auto& bone1         = _bone1;
  Vector3& target     = targetPosition;
  Vector3& poleTarget = poleTargetPosition;

  auto& mat1 = BoneIKController::_tmpMats[0];
  auto& mat2 = BoneIKController::_tmpMats[1];

  if (targetMesh) {
    target.copyFrom(*targetMesh->getAbsolutePosition());
  }

  if (poleTargetBone) {
    poleTargetBone->getAbsolutePositionFromLocalToRef(poleTargetLocalOffset,
                                                      mesh, poleTarget);
  }
  else if (poleTargetMesh) {
    Vector3::TransformCoordinatesToRef(
      poleTargetLocalOffset, *poleTargetMesh->getWorldMatrix(), poleTarget);
  }

  auto& bonePos = BoneIKController::_tmpVecs[0];
  auto& zaxis   = BoneIKController::_tmpVecs[1];
  auto& xaxis   = BoneIKController::_tmpVecs[2];
  auto& yaxis   = BoneIKController::_tmpVecs[3];
  auto& upAxis  = BoneIKController::_tmpVecs[4];

  auto& _tmpQuat = BoneIKController::_tmpQuat;

  bone1->getAbsolutePositionToRef(mesh, bonePos);

  poleTarget.subtractToRef(bonePos, upAxis);

  if (std_util::almost_equal(upAxis.x, 0.f)
      && std_util::almost_equal(upAxis.y, 0.f)
      && std_util::almost_equal(upAxis.z, 0.f)) {
    upAxis.y = 1.f;
  }
  else {
    upAxis.normalize();
  }

  target.subtractToRef(bonePos, yaxis);
  yaxis.normalize();

  Vector3::CrossToRef(yaxis, upAxis, zaxis);
  zaxis.normalize();

  Vector3::CrossToRef(yaxis, zaxis, xaxis);
  xaxis.normalize();

  Matrix::FromXYZAxesToRef(xaxis, yaxis, zaxis, mat1);

  const float a = _bone1Length;
  const float b = _bone2Length;

  float c = Vector3::Distance(bonePos, target);

  if (_maxReach > 0.f) {
    c = std::min(_maxReach, c);
  }

  float acosa = (b * b + c * c - a * a) / (2.f * b * c);
  float acosb = (c * c + a * a - b * b) / (2.f * c * a);

  if (acosa > 1.f) {
    acosa = 1.f;
  }

  if (acosb > 1.f) {
    acosb = 1.f;
  }

  if (acosa < -1.f) {
    acosa = -1.f;
  }

  if (acosb < -1.f) {
    acosb = -1.f;
  }

  const float angA = std::acos(acosa);
  const float angB = std::acos(acosb);

  float angC = -angA - angB;

  if (_rightHandedSystem) {
    Matrix::RotationYawPitchRollToRef(0.f, 0.f, _adjustRoll, mat2);
    mat2.multiplyToRef(mat1, mat1);

    Matrix::RotationAxisToRef(_bendAxis, angB, mat2);
    mat1.multiplyToRef(mat2, mat1);
  }
  else {
    auto& _tmpVec = BoneIKController::_tmpVecs[5];

    _tmpVec.copyFrom(_bendAxis);
    _tmpVec.x *= -1.f;

    Matrix::RotationAxisToRef(_tmpVec, -angB, mat2);
    mat2.multiplyToRef(mat1, mat1);
  }

  if (poleAngle != 0.f) {
    Matrix::RotationAxisToRef(yaxis, poleAngle, mat2);
    mat1.multiplyToRef(mat2, mat1);
  }

  if (slerpAmount < 1.f) {
    if (!_slerping) {
      Quaternion::FromRotationMatrixToRef(_bone1Mat, _bone1Quat);
    }
    Quaternion::FromRotationMatrixToRef(mat1, _tmpQuat);
    Quaternion::SlerpToRef(_bone1Quat, _tmpQuat, slerpAmount, _bone1Quat);
    angC = _bone2Ang * (1.f - slerpAmount) + angC * slerpAmount;
    _bone1->setRotationQuaternion(_bone1Quat, Space::WORLD, mesh);
    _slerping = true;
  }
  else {
    _bone1->setRotationMatrix(mat1, Space::WORLD, mesh);
    _bone1Mat.copyFrom(mat1);
    _slerping = false;
  }

  _bone2->setAxisAngle(_bendAxis, angC, Space::LOCAL);
  _bone2Ang = angC;
}

} // end of namespace BABYLON
