#include <babylon/bones/bone_look_controller.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/meshes/abstract_mesh.h>

namespace BABYLON {

std::array<Vector3, 10> BoneLookController::_tmpVecs{
  {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero(),
   Vector3::Zero(), Vector3::Zero()}};
Quaternion BoneLookController::_tmpQuat{Quaternion::Identity()};
std::array<Matrix, 5> BoneLookController::_tmpMats{
  {Matrix::Identity(), Matrix::Identity(), Matrix::Identity(),
   Matrix::Identity(), Matrix::Identity()}};

BoneLookController::BoneLookController(
  AbstractMesh* iMesh, Bone* iBone, const Vector3& iTarget,
  const std::optional<BoneLookControllerOptions>& iOptions)
    : target{iTarget}
    , mesh{iMesh}
    , bone{iBone}
    , upAxis{Vector3::Up()}
    , upAxisSpace{Space::LOCAL}
    , adjustYaw{0.f}
    , adjustPitch{0.f}
    , adjustRoll{0.f}
    , slerpAmount{1.f}
    , minYaw{this, &BoneLookController::get_minYaw,
             &BoneLookController::set_minYaw}
    , maxYaw{this, &BoneLookController::get_maxYaw,
             &BoneLookController::set_maxYaw}
    , minPitch{this, &BoneLookController::get_minPitch,
               &BoneLookController::set_minPitch}
    , maxPitch{this, &BoneLookController::get_maxPitch,
               &BoneLookController::set_maxPitch}
    , _minYawSet{false}
    , _maxYawSet{false}
    , _boneQuat{Quaternion::Identity()}
    , _slerping{false}
    , _firstFrameSkipped{false}
    , _fowardAxis{Vector3::Forward()}
{
  if (iOptions) {
    const auto& options = iOptions.value();
    if (options.adjustYaw) {
      adjustYaw = options.adjustYaw.value();
    }

    if (options.adjustPitch) {
      adjustPitch = options.adjustPitch.value();
    }

    if (options.adjustRoll) {
      adjustRoll = options.adjustRoll.value();
    }

    if (options.maxYaw.has_value()) {
      maxYaw = options.maxYaw.value();
    }
    else {
      maxYaw = Math::PI;
    }

    if (options.minYaw.has_value()) {
      minYaw = options.minYaw.value();
    }
    else {
      minYaw = -Math::PI;
    }

    if (options.maxPitch.has_value()) {
      maxPitch = options.maxPitch.value();
    }
    else {
      maxPitch = Math::PI;
    }

    if (options.minPitch.has_value()) {
      minPitch = options.minPitch.value();
    }
    else {
      minPitch = -Math::PI;
    }

    if (options.slerpAmount.has_value()) {
      slerpAmount = options.slerpAmount.value();
    }

    if (options.upAxis.has_value()) {
      upAxis = options.upAxis.value();
    }

    if (options.upAxisSpace.has_value()) {
      upAxisSpace = options.upAxisSpace.value();
    }

    if (options.yawAxis.has_value() || options.pitchAxis.has_value()) {

      auto newYawAxis   = Axis::Y();
      auto newPitchAxis = Axis::X();

      if (options.yawAxis.has_value()) {
        newYawAxis = options.yawAxis.value();
        newYawAxis.normalize();
      }

      if (options.pitchAxis.has_value()) {
        newPitchAxis = options.pitchAxis.value();
        newPitchAxis.normalize();
      }

      auto newRollAxis = Vector3::Cross(newPitchAxis, newYawAxis);

      Matrix _transformYawPitchTmp = Matrix::Identity();
      Matrix::FromXYZAxesToRef(newPitchAxis, newYawAxis, newRollAxis,
                               _transformYawPitchTmp);

      _transformYawPitchInv = _transformYawPitchTmp;
      _transformYawPitchTmp.invert();
      _transformYawPitch = _transformYawPitchTmp;
    }
  }

  if (!bone->getParent() && upAxisSpace == Space::BONE) {
    upAxisSpace = Space::LOCAL;
  }
}

BoneLookController::~BoneLookController() = default;

float BoneLookController::get_minYaw() const
{
  return _minYaw;
}

void BoneLookController::set_minYaw(float value)
{
  _minYaw    = value;
  _minYawSet = true;
  _minYawSin = std::sin(value);
  _minYawCos = std::cos(value);
  if (_maxYawSet) {
    _midYawConstraint = _getAngleDiff(_minYaw, _maxYaw) * 0.5f + _minYaw;
    _yawRange         = _maxYaw - _minYaw;
  }
}

float BoneLookController::get_maxYaw() const
{
  return _maxYaw;
}

void BoneLookController::set_maxYaw(float value)
{
  _maxYaw    = value;
  _minYawSet = true;
  _maxYawSin = std::sin(value);
  _maxYawCos = std::cos(value);
  if (_minYawSet) {
    _midYawConstraint = _getAngleDiff(_minYaw, _maxYaw) * 0.5f + _minYaw;
    _yawRange         = _maxYaw - _minYaw;
  }
}

float BoneLookController::get_minPitch() const
{
  return _minPitch;
}

void BoneLookController::set_minPitch(float value)
{
  _minPitch    = value;
  _minPitchTan = std::tan(value);
}

float BoneLookController::get_maxPitch() const
{
  return _maxPitch;
}

void BoneLookController::set_maxPitch(float value)
{
  _maxPitch    = value;
  _maxPitchTan = std::tan(value);
}

void BoneLookController::update()
{
  // skip the first frame when slerping so that the mesh rotation is correct
  if (slerpAmount < 1.f && !_firstFrameSkipped) {
    _firstFrameSkipped = true;
    return;
  }

  auto& bonePos = BoneLookController::_tmpVecs[0];
  bone->getAbsolutePositionToRef(mesh, bonePos);

  auto& _tmpMat1 = BoneLookController::_tmpMats[0];
  auto& _tmpMat2 = BoneLookController::_tmpMats[1];

  auto parentBone = bone->getParent();

  auto& _upAxis = BoneLookController::_tmpVecs[1];
  _upAxis.copyFrom(upAxis);

  if (upAxisSpace == Space::BONE && parentBone) {
    if (_transformYawPitch) {
      Vector3::TransformCoordinatesToRef(_upAxis, _transformYawPitchInv,
                                         _upAxis);
    }
    parentBone->getDirectionToRef(_upAxis, _upAxis, mesh);
  }
  else if (upAxisSpace == Space::LOCAL) {
    mesh->getDirectionToRef(_upAxis, _upAxis);
    if (mesh->scaling().x != 1.f || mesh->scaling().y != 1.f
        || mesh->scaling().z != 1.f) {
      _upAxis.normalize();
    }
  }

  bool checkYaw   = false;
  bool checkPitch = false;

  if (!stl_util::almost_equal(_maxYaw, Math::PI)
      || !stl_util::almost_equal(_minYaw, -Math::PI)) {
    checkYaw = true;
  }
  if (!stl_util::almost_equal(_maxPitch, Math::PI)
      || !stl_util::almost_equal(_minPitch, -Math::PI)) {
    checkPitch = true;
  }

  if (checkYaw || checkPitch) {

    auto& spaceMat    = BoneLookController::_tmpMats[2];
    auto& spaceMatInv = BoneLookController::_tmpMats[3];

    if (upAxisSpace == Space::BONE && _upAxis.y == 1.f && parentBone) {
      parentBone->getRotationMatrixToRef(spaceMat, Space::WORLD, mesh);
    }
    else if (upAxisSpace == Space::LOCAL && _upAxis.y == 1.f && !parentBone) {
      spaceMat.copyFrom(mesh->getWorldMatrix());
    }
    else {

      auto& forwardAxis = BoneLookController::_tmpVecs[2];
      forwardAxis.copyFrom(_fowardAxis);

      if (_transformYawPitch) {
        Vector3::TransformCoordinatesToRef(forwardAxis, _transformYawPitchInv,
                                           forwardAxis);
      }

      if (parentBone) {
        parentBone->getDirectionToRef(forwardAxis, forwardAxis, mesh);
      }
      else {
        mesh->getDirectionToRef(forwardAxis, forwardAxis);
      }

      auto rightAxis = Vector3::Cross(_upAxis, forwardAxis);
      rightAxis.normalize();
      forwardAxis = Vector3::Cross(rightAxis, _upAxis);

      Matrix::FromXYZAxesToRef(rightAxis, upAxis, forwardAxis, spaceMat);
    }

    spaceMat.invertToRef(spaceMatInv);

    bool xzlenSet = false;
    float xzlen   = 0.f;

    if (checkPitch) {
      auto& localTarget = BoneLookController::_tmpVecs[3];
      target.subtractToRef(bonePos, localTarget);
      Vector3::TransformCoordinatesToRef(localTarget, spaceMatInv, localTarget);

      xzlen          = std::sqrt(localTarget.x * localTarget.x
                        + localTarget.z * localTarget.z);
      float pitch    = std::atan2(localTarget.y, xzlen);
      float newPitch = pitch;

      if (pitch > _maxPitch) {
        localTarget.y = _maxPitchTan * xzlen;
        newPitch      = _maxPitch;
      }
      else if (pitch < _minPitch) {
        localTarget.y = _minPitchTan * xzlen;
        newPitch      = _minPitch;
      }

      if (!stl_util::almost_equal(pitch, newPitch)) {
        Vector3::TransformCoordinatesToRef(localTarget, spaceMat, localTarget);
        localTarget.addInPlace(bonePos);
        target = localTarget;
      }
    }

    if (checkYaw) {
      auto& localTarget = BoneLookController::_tmpVecs[4];
      target.subtractToRef(bonePos, localTarget);
      Vector3::TransformCoordinatesToRef(localTarget, spaceMatInv, localTarget);

      float yaw    = std::atan2(localTarget.x, localTarget.z);
      float newYaw = yaw;

      if (yaw > _maxYaw || yaw < _minYaw) {

        if (!xzlenSet) {
          xzlen    = std::sqrt(localTarget.x * localTarget.x
                            + localTarget.z * localTarget.z);
          xzlenSet = true;
        }

        if (_yawRange > Math::PI) {
          if (_isAngleBetween(yaw, _maxYaw, _midYawConstraint)) {
            localTarget.z = _maxYawCos * xzlen;
            localTarget.x = _maxYawSin * xzlen;
            newYaw        = _maxYaw;
          }
          else if (_isAngleBetween(yaw, _midYawConstraint, _minYaw)) {
            localTarget.z = _minYawCos * xzlen;
            localTarget.x = _minYawSin * xzlen;
            newYaw        = _minYaw;
          }
        }
        else {
          if (yaw > _maxYaw) {
            localTarget.z = _maxYawCos * xzlen;
            localTarget.x = _maxYawSin * xzlen;
            newYaw        = _maxYaw;
          }
          else if (yaw < _minYaw) {
            localTarget.z = _minYawCos * xzlen;
            localTarget.x = _minYawSin * xzlen;
            newYaw        = _minYaw;
          }
        }
      }

      if (_slerping && _yawRange > Math::PI) {
        // are we going to be crossing into the min/max region?
        auto& boneFwd = BoneLookController::_tmpVecs[8];
        boneFwd.copyFrom(Axis::Z());
        if (_transformYawPitch) {
          Vector3::TransformCoordinatesToRef(boneFwd, _transformYawPitchInv,
                                             boneFwd);
        }

        auto& boneRotMat = BoneLookController::_tmpMats[4];
        _boneQuat.toRotationMatrix(boneRotMat);
        mesh->getWorldMatrix().multiplyToRef(boneRotMat, boneRotMat);
        Vector3::TransformCoordinatesToRef(boneFwd, boneRotMat, boneFwd);
        Vector3::TransformCoordinatesToRef(boneFwd, spaceMatInv, boneFwd);

        float boneYaw      = std::atan2(boneFwd.x, boneFwd.z);
        float angBtwTar    = _getAngleBetween(boneYaw, yaw);
        float angBtwMidYaw = _getAngleBetween(boneYaw, _midYawConstraint);

        if (angBtwTar > angBtwMidYaw) {

          if (!xzlenSet) {
            xzlen    = std::sqrt(localTarget.x * localTarget.x
                              + localTarget.z * localTarget.z);
            xzlenSet = true;
          }

          float angBtwMax = _getAngleBetween(boneYaw, _maxYaw);
          float angBtwMin = _getAngleBetween(boneYaw, _minYaw);

          if (angBtwMin < angBtwMax) {
            newYaw        = boneYaw + Math::PI * 0.75f;
            localTarget.z = std::cos(newYaw) * xzlen;
            localTarget.x = std::sin(newYaw) * xzlen;
          }
          else {
            newYaw        = boneYaw - Math::PI * 0.75f;
            localTarget.z = std::cos(newYaw) * xzlen;
            localTarget.x = std::sin(newYaw) * xzlen;
          }
        }
      }

      if (!stl_util::almost_equal(yaw, newYaw)) {
        Vector3::TransformCoordinatesToRef(localTarget, spaceMat, localTarget);
        localTarget.addInPlace(bonePos);
        target = localTarget;
      }
    }
  }

  auto& zaxis    = BoneLookController::_tmpVecs[5];
  auto& xaxis    = BoneLookController::_tmpVecs[6];
  auto& yaxis    = BoneLookController::_tmpVecs[7];
  auto& iTmpQuat = BoneLookController::_tmpQuat;

  target.subtractToRef(bonePos, zaxis);
  zaxis.normalize();
  Vector3::CrossToRef(_upAxis, zaxis, xaxis);
  xaxis.normalize();
  Vector3::CrossToRef(zaxis, xaxis, yaxis);
  yaxis.normalize();
  Matrix::FromXYZAxesToRef(xaxis, yaxis, zaxis, _tmpMat1);

  if (xaxis.x == 0.f && xaxis.y == 0.f && xaxis.z == 0.f) {
    return;
  }

  if (yaxis.x == 0.f && yaxis.y == 0.f && yaxis.z == 0.f) {
    return;
  }

  if (zaxis.x == 0.f && zaxis.y == 0.f && zaxis.z == 0.f) {
    return;
  }

  if (adjustYaw != 0.f || adjustPitch != 0.f || adjustRoll != 0.f) {
    Matrix::RotationYawPitchRollToRef(adjustYaw, adjustPitch, adjustRoll,
                                      _tmpMat2);
    _tmpMat2.multiplyToRef(_tmpMat1, _tmpMat1);
  }

  if (slerpAmount < 1) {
    if (!_slerping) {
      bone->getRotationQuaternionToRef(_boneQuat, Space::WORLD, mesh);
    }
    if (_transformYawPitch) {
      Matrix transformYawPitch = _transformYawPitch.value();
      transformYawPitch.multiplyToRef(_tmpMat1, _tmpMat1);
      _transformYawPitch = std::move(transformYawPitch);
    }
    Quaternion::FromRotationMatrixToRef(_tmpMat1, iTmpQuat);
    Quaternion::SlerpToRef(_boneQuat, iTmpQuat, slerpAmount, _boneQuat);

    bone->setRotationQuaternion(_boneQuat, Space::WORLD, mesh);
    _slerping = true;
  }
  else {
    if (_transformYawPitch) {
      Matrix transformYawPitch = _transformYawPitch.value();
      transformYawPitch.multiplyToRef(_tmpMat1, _tmpMat1);
      _transformYawPitch = std::move(transformYawPitch);
    }
    bone->setRotationMatrix(_tmpMat1, Space::WORLD, mesh);
    _slerping = false;
  }
}

float BoneLookController::_getAngleDiff(float ang1, float ang2) const
{
  float angDiff = ang2 - ang1;
  angDiff       = fmodf(angDiff, Math::PI2);

  if (angDiff > Math::PI) {
    angDiff -= Math::PI2;
  }
  else if (angDiff < -Math::PI) {
    angDiff += Math::PI2;
  }

  return angDiff;
}

float BoneLookController::_getAngleBetween(float ang1, float ang2) const
{
  ang1 = fmodf(ang1, Math::PI2);
  ang1 = (ang1 < 0.f) ? ang1 + Math::PI2 : ang1;

  ang2 = fmodf(ang2, Math::PI2);
  ang2 = (ang2 < 0.f) ? ang2 + Math::PI2 : ang2;

  float ab = 0.f;

  if (ang1 < ang2) {
    ab = ang2 - ang1;
  }
  else {
    ab = ang1 - ang2;
  }

  if (ab > Math::PI) {
    ab = Math::PI2 - ab;
  }

  return ab;
}

bool BoneLookController::_isAngleBetween(float ang, float ang1,
                                         float ang2) const
{
  ang  = fmodf(ang, Math::PI2);
  ang  = (ang < 0.f) ? ang + Math::PI2 : ang;
  ang1 = fmodf(ang1, Math::PI2);
  ang1 = (ang1 < 0.f) ? ang1 + Math::PI2 : ang1;
  ang2 = fmodf(ang2, Math::PI2);
  ang2 = (ang2 < 0.f) ? ang2 + Math::PI2 : ang2;

  if (ang1 < ang2) {
    if (ang > ang1 && ang < ang2) {
      return true;
    }
  }
  else {
    if (ang > ang2 && ang < ang1) {
      return true;
    }
  }
  return false;
}

} // end of namespace BABYLON
