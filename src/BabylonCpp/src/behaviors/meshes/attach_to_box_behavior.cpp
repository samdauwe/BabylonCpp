#include <babylon/behaviors/meshes/attach_to_box_behavior.h>

#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

AttachToBoxBehavior::AttachToBoxBehavior(const TransformNodePtr& iUi)
    : distanceAwayFromFace{0.15f}
    , distanceAwayFromBottomOfFace{0.15f}
    , ui{iUi}
    , _faceVectors{{FaceDirectionInfo{Vector3::Up()},
                    FaceDirectionInfo{Vector3::Down()},
                    FaceDirectionInfo{Vector3::Left()},
                    FaceDirectionInfo{Vector3::Right()},
                    FaceDirectionInfo{Vector3::Forward()},
                    FaceDirectionInfo{Vector3::Forward().scaleInPlace(-1.f)}}}
    , _target{nullptr}
    , _scene{nullptr}
    , _onRenderObserver{nullptr}
    , _zeroVector{Vector3::Zero()}
{
  /* Does nothing */
}

AttachToBoxBehavior::~AttachToBoxBehavior() = default;

void AttachToBoxBehavior::init()
{
  /* Does nothing */
}

FaceDirectionInfo
AttachToBoxBehavior::_closestFace(const Vector3& targetDirection)
{
  // Go over each face and calculate the angle between the face's normal and
  // targetDirection
  for (auto& v : _faceVectors) {
    if (!_target->rotationQuaternion()) {
      _target->rotationQuaternion = Quaternion::RotationYawPitchRoll(
        _target->rotation().y, _target->rotation().x, _target->rotation().z);
    }
    _target->rotationQuaternion()->toRotationMatrix(_tmpMatrix);
    Vector3::TransformCoordinatesToRef(v.direction, _tmpMatrix,
                                       v.rotatedDirection);
    v.diff = Vector3::GetAngleBetweenVectors(
      v.rotatedDirection, targetDirection,
      Vector3::Cross(v.rotatedDirection, targetDirection));
  }

  // Return the face information of the one with the normal closeset to target
  // direction
  const auto determineMin
    = [](const FaceDirectionInfo& min, const FaceDirectionInfo& p) {
        if (min.ignore) {
          return p;
        }
        else if (p.ignore) {
          return min;
        }
        else {
          return min.diff < p.diff ? min : p;
        }
      };

  auto min = _faceVectors[0];
  for (auto& p : _faceVectors) {
    determineMin(min, p);
  }

  return min;
}

void AttachToBoxBehavior::_lookAtToRef(const Vector3& pos, Quaternion& ref,
                                       const Vector3 up)
{
  Matrix::LookAtLHToRef(_zeroVector, pos, up, _lookAtTmpMatrix);
  _lookAtTmpMatrix.invert();
  Quaternion::FromRotationMatrixToRef(_lookAtTmpMatrix, ref);
}

void AttachToBoxBehavior::attach(
  const MeshPtr& target,
  const std::function<bool(const AbstractMeshPtr& m)>& /*predicate*/)
{
  _target = target;
  _scene  = _target->getScene();

  // Every frame, update the app bars position
  _onRenderObserver = _scene->onBeforeRenderObservable.add(
    [this, target](Scene* /*scene*/, EventState& /*es*/) {
      if (!_scene->activeCamera()) {
        return;
      }

      // Find the face closest to the cameras position
      auto cameraPos = _scene->activeCamera()->position();
      auto facing    = _closestFace(cameraPos.subtract(target->position()));
      if (_scene->activeCamera()->leftCamera()) {
        _scene->activeCamera()
          ->leftCamera()
          ->computeWorldMatrix()
          .getRotationMatrixToRef(_tmpMatrix);
      }
      else {
        _scene->activeCamera()->computeWorldMatrix().getRotationMatrixToRef(
          _tmpMatrix);
      }

      // Get camera up direction
      Vector3::TransformCoordinatesToRef(Vector3::Up(), _tmpMatrix, _tmpVector);
      // Ignore faces to not select a parallel face for the up vector of the UI
      for (auto& v : _faceVectors) {
        if (facing.direction.x != 0.f && v.direction.x != 0.f) {
          v.ignore = true;
        }
        if (facing.direction.y != 0.f && v.direction.y != 0.f) {
          v.ignore = true;
        }
        if (facing.direction.z != 0.f && v.direction.z != 0.f) {
          v.ignore = true;
        }
      }
      auto facingUp = _closestFace(_tmpVector);
      // Unignore faces
      for (auto& v : _faceVectors) {
        v.ignore = false;
      }

      // Position the app bar on that face
      ui->position().copyFrom(target->position());
      if (facing.direction.x != 0.f) {
        facing.rotatedDirection.scaleToRef(
          (target->scaling().x / 2.f) + distanceAwayFromFace, _tmpVector);
        ui->position().addInPlace(_tmpVector);
      }
      if (facing.direction.y != 0.f) {
        facing.rotatedDirection.scaleToRef(
          (target->scaling().y / 2.f) + distanceAwayFromFace, _tmpVector);
        ui->position().addInPlace(_tmpVector);
      }
      if (facing.direction.z != 0.f) {
        facing.rotatedDirection.scaleToRef(
          (target->scaling().z / 2.f) + distanceAwayFromFace, _tmpVector);
        ui->position().addInPlace(_tmpVector);
      }

      // Rotate to be oriented properly to the camera
      if (!ui->rotationQuaternion()) {
        ui->rotationQuaternion = Quaternion::RotationYawPitchRoll(
          ui->rotation().y, ui->rotation().x, ui->rotation().z);
      }
      facing.rotatedDirection.scaleToRef(-1, _tmpVector);
      _lookAtToRef(_tmpVector, *ui->rotationQuaternion(),
                   facingUp.rotatedDirection);

      // Place ui the correct distance from the bottom of the mesh
      if (facingUp.direction.x != 0.f) {
        ui->up().scaleToRef(
          distanceAwayFromBottomOfFace - target->scaling().x / 2.f, _tmpVector);
      }
      if (facingUp.direction.y != 0.f) {
        ui->up().scaleToRef(
          distanceAwayFromBottomOfFace - target->scaling().y / 2.f, _tmpVector);
      }
      if (facingUp.direction.z != 0.f) {
        ui->up().scaleToRef(
          distanceAwayFromBottomOfFace - target->scaling().z / 2.f, _tmpVector);
      }
      ui->position().addInPlace(_tmpVector);
    });
}

void AttachToBoxBehavior::detach()
{
  _scene->onBeforeRenderObservable.remove(_onRenderObserver);
}

} // end of namespace BABYLON
