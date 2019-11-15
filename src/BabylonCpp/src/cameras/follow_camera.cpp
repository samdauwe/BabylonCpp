#include <babylon/cameras/follow_camera.h>

#include <babylon/meshes/abstract_mesh.h>
#include <babylon/misc/tools.h>

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
    , lowerRadiusLimit{std::nullopt}
    , upperRadiusLimit{std::nullopt}
    , rotationOffset{0.f}
    , lowerRotationOffsetLimit{std::nullopt}
    , upperRotationOffsetLimit{std::nullopt}
    , heightOffset{4.f}
    , lowerHeightOffsetLimit{std::nullopt}
    , upperHeightOffsetLimit{std::nullopt}
    , cameraAcceleration{0.05f}
    , maxCameraSpeed{20.f}
    , lockedTarget{iLockedTarget}
    , inputs{std::make_unique<FollowCameraInputsManager>(this)}
{
  inputs->addKeyboard().addMouseWheel().addPointers();
  // Uncomment the following line when the relevant handlers have been
  // implemented.
  // inputs->addKeyboard().addMouseWheel().addPointers().addVRDeviceOrientation();
}

FollowCamera::~FollowCamera() = default;

Type FollowCamera::type() const
{
  return Type::FOLLOWCAMERA;
}

void FollowCamera::_follow(const AbstractMeshPtr& cameraTarget)
{
  if (!cameraTarget) {
    return;
  }

  auto yRotation = 0.f;
  if (cameraTarget->rotationQuaternion()) {
    Matrix rotMatrix;
    cameraTarget->rotationQuaternion()->toRotationMatrix(rotMatrix);
    yRotation = std::atan2(rotMatrix.m()[8], rotMatrix.m()[10]);
  }
  else {
    yRotation = cameraTarget->rotation().y;
  }
  auto radians        = Tools::ToRadians(rotationOffset) + yRotation;
  auto targetPosition = cameraTarget->getAbsolutePosition();
  auto targetX        = targetPosition.x + std::sin(radians) * radius;
  auto targetZ        = targetPosition.z + std::cos(radians) * radius;

  auto dx = targetX - position().x;
  auto dy = (targetPosition.y + heightOffset) - position().y;
  auto dz = targetZ - position().z;
  auto vx = dx * cameraAcceleration * 2.f; // this is set to .05
  auto vy = dy * cameraAcceleration;
  auto vz = dz * cameraAcceleration * 2.f;

  if (vx > maxCameraSpeed || vx < -maxCameraSpeed) {
    vx = vx < 1.f ? -maxCameraSpeed : maxCameraSpeed;
  }

  if (vy > maxCameraSpeed || vy < -maxCameraSpeed) {
    vy = vy < 1.f ? -maxCameraSpeed : maxCameraSpeed;
  }

  if (vz > maxCameraSpeed || vz < -maxCameraSpeed) {
    vz = vz < 1.f ? -maxCameraSpeed : maxCameraSpeed;
  }

  position = Vector3(position().x + vx, position().y + vy, position().z + vz);
  setTarget(targetPosition);
}

void FollowCamera::attachControl(ICanvas* element, bool noPreventDefault,
                                 bool /*useCtrlForPanning*/,
                                 MouseButtonType /*panningMouseButton*/
)
{
  inputs->attachElement(element, noPreventDefault);

  _reset = []() -> void {};
}

void FollowCamera::detachControl(ICanvas* element)
{
  inputs->detachElement(element);

  if (_reset) {
    _reset();
  }
}

void FollowCamera::_checkInputs()
{
  inputs->checkInputs();
  _checkLimits();
  TargetCamera::_checkInputs();
  if (lockedTarget) {
    _follow(lockedTarget);
  }
}

void FollowCamera::_checkLimits()
{
  if (lowerRadiusLimit.has_value() && radius < *lowerRadiusLimit) {
    radius = *lowerRadiusLimit;
  }
  if (upperRadiusLimit.has_value() && radius > *upperRadiusLimit) {
    radius = *upperRadiusLimit;
  }

  if (lowerHeightOffsetLimit.has_value()
      && heightOffset < *lowerHeightOffsetLimit) {
    heightOffset = *lowerHeightOffsetLimit;
  }
  if (upperHeightOffsetLimit.has_value()
      && heightOffset > *upperHeightOffsetLimit) {
    heightOffset = *upperHeightOffsetLimit;
  }

  if (lowerRotationOffsetLimit.has_value()
      && rotationOffset < *lowerRotationOffsetLimit) {
    rotationOffset = *lowerRotationOffsetLimit;
  }
  if (upperRotationOffsetLimit.has_value()
      && rotationOffset > *upperRotationOffsetLimit) {
    rotationOffset = *upperRotationOffsetLimit;
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
