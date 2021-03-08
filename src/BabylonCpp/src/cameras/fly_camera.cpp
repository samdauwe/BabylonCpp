#include <babylon/cameras/fly_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/fly_camera_inputs_manager.h>
#include <babylon/cameras/inputs/fly_camera_keyboard_input.h>
#include <babylon/cameras/inputs/fly_camera_mouse_input.h>
#include <babylon/collisions/collider.h>
#include <babylon/collisions/collision_coordinator.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

FlyCamera::FlyCamera(const std::string& iName, const Vector3& iPosition, Scene* scene,
                     bool setActiveOnSceneIfNoneActive)
    : TargetCamera{iName, iPosition, scene, setActiveOnSceneIfNoneActive}
    , ellipsoid{Vector3(1.f, 1.f, 1.f)}
    , ellipsoidOffset{Vector3(0.f, 0.f, 0.f)}
    , checkCollisions{false}
    , applyGravity{false}
    , _trackRoll{0.f}
    , rollCorrect{100.f}
    , bankedTurn{false}
    , bankedTurnLimit{Math::PI_2}
    , bankedTurnMultiplier{1}
    , inputs{nullptr}
    , angularSensibility{this, &FlyCamera::get_angularSensibility,
                         &FlyCamera::set_angularSensibility}
    , keysForward{this, &FlyCamera::get_keysForward, &FlyCamera::set_keysForward}
    , keysBackward{this, &FlyCamera::get_keysBackward, &FlyCamera::set_keysBackward}
    , keysUp{this, &FlyCamera::get_keysUp, &FlyCamera::set_keysUp}
    , keysDown{this, &FlyCamera::get_keysDown, &FlyCamera::set_keysDown}
    , keysLeft{this, &FlyCamera::get_keysLeft, &FlyCamera::set_keysLeft}
    , keysRight{this, &FlyCamera::get_keysRight, &FlyCamera::set_keysRight}
    , onCollide{nullptr}
    , _localDirection{Vector3::Zero()}
    , _transformedDirection{Vector3::Zero()}
    , collisionMask{this, &FlyCamera::get_collisionMask, &FlyCamera::set_collisionMask}
    , _collider{nullptr}
    , _needMoveForGravity{false}
    , _oldPosition{Vector3::Zero()}
    , _diffPosition{Vector3::Zero()}
    , _newPosition{Vector3::Zero()}
    , _collisionMask{-1}
{
  cameraDirection = std::make_unique<Vector3>(Vector3::Zero());

  inputs = std::make_unique<FlyCameraInputsManager>(this);
  inputs->addKeyboard().addMouse();
}

FlyCamera::~FlyCamera() = default;

float FlyCamera::get_angularSensibility() const
{
  if (stl_util::contains(inputs->attached, "mouse")) {
    auto mouse = std::static_pointer_cast<FlyCameraMouseInput>(inputs->attached["mouse"]);
    if (mouse) {
      return mouse->angularSensibility;
    }
  }

  return 0.f;
}

void FlyCamera::set_angularSensibility(float value)
{
  if (stl_util::contains(inputs->attached, "mouse")) {
    auto mouse = std::static_pointer_cast<FlyCameraMouseInput>(inputs->attached["mouse"]);
    if (mouse) {
      mouse->angularSensibility = value;
    }
  }
}

Int32Array& FlyCamera::get_keysForward()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysForward;
    }
  }

  return _emptyKeyboardKeys;
}

void FlyCamera::set_keysForward(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysForward = value;
    }
  }
}

Int32Array& FlyCamera::get_keysBackward()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysBackward;
    }
  }

  return _emptyKeyboardKeys;
}

void FlyCamera::set_keysBackward(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysBackward = value;
    }
  }
}

Int32Array& FlyCamera::get_keysUp()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysUp;
    }
  }

  return _emptyKeyboardKeys;
}

void FlyCamera::set_keysUp(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysUp = value;
    }
  }
}

Int32Array& FlyCamera::get_keysDown()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysDown;
    }
  }

  return _emptyKeyboardKeys;
}

void FlyCamera::set_keysDown(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysDown = value;
    }
  }
}

Int32Array& FlyCamera::get_keysLeft()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysLeft;
    }
  }

  return _emptyKeyboardKeys;
}

void FlyCamera::set_keysLeft(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysLeft = value;
    }
  }
}

Int32Array& FlyCamera::get_keysRight()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysRight;
    }
  }

  return _emptyKeyboardKeys;
}

void FlyCamera::set_keysRight(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FlyCameraKeyboardInput>(inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysRight = value;
    }
  }
}

void FlyCamera::attachControl(bool noPreventDefault, bool /*useCtrlForPanning*/,
                              int /*panningMouseButton*/)
{
  inputs->attachElement(noPreventDefault);
}

void FlyCamera::detachControl(ICanvas* /*ignored*/)
{
  inputs->detachElement();

  cameraDirection = std::make_unique<Vector3>(0.f, 0.f, 0.f);
}

int FlyCamera::get_collisionMask() const
{
  return _collisionMask;
}

void FlyCamera::set_collisionMask(int mask)
{
  _collisionMask = !isNan(mask) ? mask : -1;
}

void FlyCamera::_collideWithWorld(const Vector3& displacement)
{
  Vector3 iGlobalPosition;

  if (parent) {
    iGlobalPosition = Vector3::TransformCoordinates(position, parent()->getWorldMatrix());
  }
  else {
    iGlobalPosition = position();
  }

  iGlobalPosition.subtractFromFloatsToRef(0, ellipsoid.y, 0, _oldPosition);
  _oldPosition.addInPlace(ellipsoidOffset);
  auto& coordinator = getScene()->collisionCoordinator();

  if (!_collider) {
    _collider = coordinator->createCollider();
  }

  _collider->_radius       = ellipsoid;
  _collider->collisionMask = _collisionMask;

  // No need for clone, as long as gravity is not on.
  auto actualDisplacement = displacement;

  // Add gravity to direction to prevent dual-collision checking.
  if (applyGravity) {
    // This prevents mending with cameraDirection, a global variable of the fly
    // camera class.
    actualDisplacement = displacement.add(getScene()->gravity);
  }

  coordinator->getNewPosition(
    _oldPosition, actualDisplacement, _collider, 3, nullptr,
    [this](size_t collisionIndex, Vector3& newPosition, const AbstractMeshPtr& collidedMesh) {
      _onCollisionPositionChange(collisionIndex, newPosition, collidedMesh);
    },
    uniqueId);
}

void FlyCamera::_onCollisionPositionChange(size_t /*collisionId*/, const Vector3& newPosition,
                                           const AbstractMeshPtr& collidedMesh)
{
  const auto updatePosition = [this, &collidedMesh](const Vector3& newPos) -> void {
    _newPosition.copyFrom(newPos);

    _newPosition.subtractToRef(_oldPosition, _diffPosition);

    if (_diffPosition.length() > Engine::CollisionsEpsilon) {
      position().addInPlace(_diffPosition);
      if (onCollide && collidedMesh) {
        onCollide(collidedMesh);
      }
    }
  };

  updatePosition(newPosition);
}

void FlyCamera::_checkInputs()
{
  if (!_localDirection.has_value()) {
    _localDirection       = Vector3::Zero();
    _transformedDirection = Vector3::Zero();
  }

  inputs->checkInputs();

  TargetCamera::_checkInputs();
}

bool FlyCamera::_decideIfNeedsToMove()
{
  return _needMoveForGravity || std::abs(cameraDirection->x) > 0.f
         || std::abs(cameraDirection->y) > 0.f || std::abs(cameraDirection->z) > 0.f;
}

void FlyCamera::_updatePosition()
{
  if (checkCollisions && getScene()->collisionsEnabled) {
    _collideWithWorld(*cameraDirection);
  }
  else {
    TargetCamera::_updatePosition();
  }
}

void FlyCamera::restoreRoll(float rate)
{
  auto limit = _trackRoll;   // Target Roll.
  auto z     = rotation().z; // Current Roll.
  auto delta = limit - z;    // Difference in Roll.

  auto minRad = 0.001f; // Tenth of a radian is a barely noticable difference.

  // If the difference is noticable, restore the Roll.
  if (std::abs(delta) >= minRad) {
    // Change Z rotation towards the target Roll.
    rotation().z += delta / rate;

    // Match when near enough.
    if (std::abs(limit - rotation().z) <= minRad) {
      rotation().z = limit;
    }
  }
}

void FlyCamera::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  inputs->clear();
  TargetCamera::dispose(doNotRecurse, disposeMaterialAndTextures);
}

std::string FlyCamera::getClassName() const
{
  return "FlyCamera";
}

} // namespace BABYLON
