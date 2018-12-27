#include <babylon/cameras/follow_camera.h>

#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

bool FollowCamera::NodeConstructorAdded = false;

void FollowCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "FollowCamera", [](const std::string& iName, Scene* scene,
                       const std::optional<json>& /*options*/) {
      return FollowCamera::New(iName, Vector3::Zero(), scene);
    });
  FollowCamera::NodeConstructorAdded = true;
}

FollowCamera::FollowCamera(const std::string& iName, const Vector3& iPosition,
                           Scene* scene, const AbstractMeshPtr& iLockedTarget)
    : TargetCamera{iName, iPosition, scene}
    , radius{12.f}
    , rotationOffset{0.f}
    , heightOffset{4.f}
    , cameraAcceleration{0.05f}
    , maxCameraSpeed{20.f}
    , lockedTarget{iLockedTarget}
{
}

FollowCamera::~FollowCamera()
{
}

IReflect::Type FollowCamera::type() const
{
  return IReflect::Type::FOLLOWCAMERA;
}

void FollowCamera::_follow(const AbstractMeshPtr& cameraTarget)
{
  if (!cameraTarget) {
    return;
  }

  float yRotation;
  if (cameraTarget->rotationQuaternion()) {
    Matrix rotMatrix;
    cameraTarget->rotationQuaternion()->toRotationMatrix(rotMatrix);
    yRotation = std::atan2(rotMatrix.m[8], rotMatrix.m[10]);
  }
  else {
    yRotation = cameraTarget->rotation().y;
  }
  float radians       = Tools::ToRadians(rotationOffset) + yRotation;
  auto targetPosition = cameraTarget->getAbsolutePosition();
  float targetX       = targetPosition.x + std::sin(radians) * radius;
  float targetZ       = targetPosition.z + std::cos(radians) * radius;

  float dx = targetX - position.x;
  float dy = (targetPosition.y + heightOffset) - position.y;
  float dz = targetZ - position.z;
  float vx = dx * cameraAcceleration * 2.f; // this is set to .05
  float vy = dy * cameraAcceleration;
  float vz = dz * cameraAcceleration * 2.f;

  if (vx > maxCameraSpeed || vx < -maxCameraSpeed) {
    vx = vx < 1.f ? -maxCameraSpeed : maxCameraSpeed;
  }

  if (vy > maxCameraSpeed || vy < -maxCameraSpeed) {
    vy = vy < 1.f ? -maxCameraSpeed : maxCameraSpeed;
  }

  if (vz > maxCameraSpeed || vz < -maxCameraSpeed) {
    vz = vz < 1.f ? -maxCameraSpeed : maxCameraSpeed;
  }

  position = Vector3(position.x + vx, position.y + vy, position.z + vz);
  setTarget(targetPosition);
}

void FollowCamera::_checkInputs()
{
  TargetCamera::_checkInputs();
  if (lockedTarget) {
    _follow(lockedTarget);
  }
}

const std::string FollowCamera::getClassName() const
{
  return "FollowCamera";
}

json FollowCamera::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
