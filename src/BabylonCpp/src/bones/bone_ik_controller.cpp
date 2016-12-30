#include <babylon/bones/bone_ik_controller.h>

#include <babylon/bones/bone.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

BoneIKController::BoneIKController(AbstractMesh* iMesh, Bone* bone,
                                   AbstractMesh* iTarget,
                                   AbstractMesh* iPoleTarget, float iPoleAngle)
    : target{iTarget}
    , poleTarget{iPoleTarget}
    , poleAngle{iPoleAngle}
    , mesh{iMesh}
    , _bone1{bone->getParent()}
    , _bone2{bone}
    , _maxAngle{Math::PI}
    , _tmpVec1{Vector3::Zero()}
    , _tmpVec2{Vector3::Zero()}
    , _tmpVec3{Vector3::Zero()}
    , _tmpVec4{Vector3::Zero()}
    , _tmpVec5{Vector3::Zero()}
    , _tmpMat1{Matrix::Identity()}
    , _tmpMat2{Matrix::Identity()}
    , _rightHandedSystem{false}
{
  target->computeWorldMatrix(true);
  poleTarget->computeWorldMatrix(true);

  if (bone->getAbsoluteTransform().determinant() > 0.f) {
    _rightHandedSystem = true;
  }

  if (_bone1->length) {

    Vector3 boneScale1 = _bone1->getScale();
    Vector3 boneScale2 = _bone2->getScale();

    _bone1Length = _bone1->length * boneScale1.y;
    _bone2Length = _bone2->length * boneScale2.y;
  }
  else if (!_bone1->children.empty() && _bone1->children[0]) {

    mesh->computeWorldMatrix(true);

    Vector3 pos1 = _bone2->children[0]->getAbsolutePosition(iMesh);
    Vector3 pos2 = _bone2->getAbsolutePosition(iMesh);
    Vector3 pos3 = _bone1->getAbsolutePosition(iMesh);

    _bone1Length = Vector3::Distance(pos1, pos2);
    _bone2Length = Vector3::Distance(pos2, pos3);
  }

  setMaxAngle(Math::PI);
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

  float a = _bone1Length;
  float b = _bone2Length;

  _maxReach = std::sqrt(a * a + b * b - 2 * a * b * std::cos(ang));
}

void BoneIKController::update()
{
  auto& bone1         = _bone1;
  Vector3 _target     = *target->getAbsolutePosition();
  Vector3 _poleTarget = *poleTarget->getAbsolutePosition();

  Vector3& bonePos = _tmpVec1;
  Vector3& zaxis   = _tmpVec2;
  Vector3& xaxis   = _tmpVec3;
  Vector3& yaxis   = _tmpVec4;
  Vector3& upAxis  = _tmpVec5;
  Matrix& mat1     = _tmpMat1;
  Matrix& mat2     = _tmpMat2;

  bone1->getAbsolutePositionToRef(mesh, bonePos);

  _poleTarget.subtractToRef(bonePos, upAxis);

  if (std_util::almost_equal(upAxis.x, 0.f)
      && std_util::almost_equal(upAxis.y, 0.f)
      && std_util::almost_equal(upAxis.z, 0.f)) {
    upAxis.y = 1;
  }
  else {
    upAxis.normalize();
  }

  _target.subtractToRef(bonePos, yaxis);
  yaxis.normalize();

  Vector3::CrossToRef(yaxis, upAxis, zaxis);
  zaxis.normalize();

  Vector3::CrossToRef(yaxis, zaxis, xaxis);
  xaxis.normalize();

  Matrix::FromXYZAxesToRef(xaxis, yaxis, zaxis, mat1);

  float a = _bone1Length;
  float b = _bone2Length;

  float c = Vector3::Distance(bonePos, _target);

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

  float angA = std::acos(acosa);
  float angB = std::acos(acosb);

  float angC = -angA - angB;

  Vector3& bendAxis = _tmpVec1;
  bendAxis.x        = 0;
  bendAxis.y        = 0;
  bendAxis.z        = 0;

  if (_rightHandedSystem) {

    bendAxis.z = 1;

    Matrix::RotationYawPitchRollToRef(0, 0, angB + Math::PI * 0.5f, mat2);
    mat2.multiplyToRef(mat1, mat1);

    Matrix::RotationAxisToRef(yaxis, poleAngle + Math::PI, mat2);
    mat1.multiplyToRef(mat2, mat1);
  }
  else {

    bendAxis.x = 1;

    Matrix::RotationYawPitchRollToRef(0, angB, 0, mat2);
    mat2.multiplyToRef(mat1, mat1);

    if (!std_util::almost_equal(poleAngle, 0.f)) {
      Matrix::RotationAxisToRef(yaxis, poleAngle, mat2);
      mat1.multiplyToRef(mat2, mat1);
    }
  }

  _bone1->setRotationMatrix(mat1, Space::WORLD, mesh);
  _bone2->setAxisAngle(bendAxis, angC, Space::LOCAL);
}

} // end of namespace BABYLON
