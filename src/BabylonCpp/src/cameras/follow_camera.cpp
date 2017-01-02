#include <babylon/cameras/follow_camera.h>

#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {

FollowCamera::FollowCamera(const std::string& iName, const Vector3& iPosition,
                           Scene* scene, AbstractMesh* iLockedTarget)
    : TargetCamera(iName, iPosition, scene)
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

Node::Type FollowCamera::type() const
{
  return Node::Type::FOLLOWCAMERA;
}

float FollowCamera::getRadians(float degrees) const
{
  return degrees * Math::PI / 180.f;
}

void FollowCamera::follow(AbstractMesh* cameraTarget)
{
  if (!cameraTarget) {
    return;
  }

  float yRotation;
  if (cameraTarget->rotationQuaternionSet()) {
    Matrix rotMatrix;
    cameraTarget->rotationQuaternion().toRotationMatrix(rotMatrix);
    yRotation = std::atan2(rotMatrix.m[8], rotMatrix.m[10]);
  }
  else {
    yRotation = cameraTarget->rotation().y;
  }
  float radians = getRadians(rotationOffset) + yRotation;
  float targetX = cameraTarget->position().x + std::sin(radians) * radius;

  float targetZ = cameraTarget->position().z + std::cos(radians) * radius;
  float dx      = targetX - position.x;
  float dy      = (cameraTarget->position().y + heightOffset) - position.y;
  float dz      = targetZ - position.z;
  float vx      = dx * cameraAcceleration * 2.f; // this is set to .05
  float vy      = dy * cameraAcceleration;
  float vz      = dz * cameraAcceleration * 2.f;

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
  setTarget(cameraTarget->position());
}

void FollowCamera::_checkInputs()
{
  TargetCamera::_checkInputs();
  follow(lockedTarget);
}

std::string FollowCamera::getTypeName() const
{
  return "FollowCamera";
}

Json::object FollowCamera::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
