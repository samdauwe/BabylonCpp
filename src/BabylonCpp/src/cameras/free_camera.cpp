#include <babylon/cameras/free_camera.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/inputs/free_camera_keyboard_move_input.h>
#include <babylon/cameras/inputs/free_camera_mouse_input.h>
#include <babylon/cameras/universal_camera.h>
#include <babylon/collisions/collider.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/misc/string_tools.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

bool FreeCamera::NodeConstructorAdded = false;

void FreeCamera::AddNodeConstructor()
{
  Node::AddNodeConstructor(
    "FreeCamera", [](const std::string& iName, Scene* scene,
                     const std::optional<json>& /*options*/) {
      // Forcing to use the Universal camera
      return UniversalCamera::New(iName, Vector3::Zero(), scene);
    });
  FreeCamera::NodeConstructorAdded = true;
}

FreeCamera::FreeCamera(const std::string& iName, const Vector3& iPosition,
                       Scene* scene, bool setActiveOnSceneIfNoneActive)
    : TargetCamera{iName, iPosition, scene, setActiveOnSceneIfNoneActive}
    , ellipsoid{Vector3(0.5f, 1.f, 0.5f)}
    , ellipsoidOffset{Vector3(0.f, 0.f, 0.f)}
    , checkCollisions{false}
    , applyGravity{false}
    , inputs{std::make_unique<FreeCameraInputsManager>(this)}
    , angularSensibility{this, &FreeCamera::get_angularSensibility,
                         &FreeCamera::set_angularSensibility}
    , keysUp{this, &FreeCamera::get_keysUp, &FreeCamera::set_keysUp}
    , keysDown{this, &FreeCamera::get_keysDown, &FreeCamera::set_keysDown}
    , keysLeft{this, &FreeCamera::get_keysLeft, &FreeCamera::set_keysLeft}
    , keysRight{this, &FreeCamera::get_keysRight, &FreeCamera::set_keysRight}
    , collisionMask{this, &FreeCamera::get_collisionMask,
                    &FreeCamera::set_collisionMask}
    , _collider{nullptr}
    , _collisionMask{-1}
    , _needMoveForGravity{false}
    , _oldPosition{Vector3::Zero()}
    , _diffPosition{Vector3::Zero()}
    , _newPosition{Vector3::Zero()}
{
  inputs->addKeyboard().addMouse();
}

FreeCamera::~FreeCamera() = default;

Type FreeCamera::type() const
{
  return Type::FREECAMERA;
}

float FreeCamera::get_angularSensibility() const
{
  if (stl_util::contains(inputs->attached, "mouse")) {
    auto mouse = std::static_pointer_cast<FreeCameraMouseInput>(
      inputs->attached["mouse"]);
    if (mouse) {
      return mouse->angularSensibility;
    }
  }

  return 0.f;
}

void FreeCamera::set_angularSensibility(float value)
{
  if (stl_util::contains(inputs->attached, "mouse")) {
    auto mouse = std::static_pointer_cast<FreeCameraMouseInput>(
      inputs->attached["mouse"]);
    if (mouse) {
      mouse->angularSensibility = value;
    }
  }
}

Int32Array& FreeCamera::get_keysUp()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysUp;
    }
  }

  return _emptyKeyboardKeys;
}

void FreeCamera::set_keysUp(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysUp = value;
    }
  }
}

Int32Array& FreeCamera::get_keysDown()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysDown;
    }
  }

  return _emptyKeyboardKeys;
}

void FreeCamera::set_keysDown(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysDown = value;
    }
  }
}

Int32Array& FreeCamera::get_keysLeft()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysLeft;
    }
  }

  return _emptyKeyboardKeys;
}

void FreeCamera::set_keysLeft(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysLeft = value;
    }
  }
}

Int32Array& FreeCamera::get_keysRight()
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      return keyboard->keysRight;
    }
  }

  return _emptyKeyboardKeys;
}

void FreeCamera::set_keysRight(const Int32Array& value)
{
  if (stl_util::contains(inputs->attached, "keyboard")) {
    auto keyboard = std::static_pointer_cast<FreeCameraKeyboardMoveInput>(
      inputs->attached["keyboard"]);
    if (keyboard) {
      keyboard->keysRight = value;
    }
  }
}

/** Controls **/
void FreeCamera::attachControl(ICanvas* canvas, bool noPreventDefault,
                               bool /*useCtrlForPanning*/,
                               MouseButtonType /*panningMouseButton*/)
{
  inputs->attachElement(canvas, noPreventDefault);
}

void FreeCamera::detachControl(ICanvas* canvas)
{
  inputs->detachElement(canvas);

  cameraDirection = std::make_unique<Vector3>(0.f, 0.f, 0.f);
  cameraRotation  = std::make_unique<Vector2>(0.f, 0.f);
}

int FreeCamera::get_collisionMask() const
{
  return _collisionMask;
}

void FreeCamera::set_collisionMask(int mask)
{
  _collisionMask = !isNan(mask) ? mask : -1;
}

void FreeCamera::_collideWithWorld(Vector3& displacement)
{
  Vector3 globalPosition_;

  if (parent()) {
    globalPosition_
      = Vector3::TransformCoordinates(position, parent()->getWorldMatrix());
  }
  else {
    globalPosition_ = position;
  }

  globalPosition_.subtractFromFloatsToRef(0, ellipsoid.y, 0, _oldPosition);
  _oldPosition.addInPlace(ellipsoidOffset);

  auto& coordinator = getScene()->collisionCoordinator();
  if (!_collider) {
    _collider = coordinator->createCollider();
  }

  _collider->_radius       = ellipsoid;
  _collider->collisionMask = _collisionMask;

  // no need for clone, as long as gravity is not on.
  auto actualDisplacement = displacement;

  // add gravity to the direction to prevent the dual-collision checking
  if (applyGravity) {
    // this prevents mending with cameraDirection, a global variable of the free
    // camera class.
    actualDisplacement = displacement.add(getScene()->gravity);
  }

  coordinator->getNewPosition(
    _oldPosition, actualDisplacement, _collider, 3, nullptr,
    [this](size_t collisionIndex, Vector3& newPosition,
           const AbstractMeshPtr& collidedMesh) {
      _onCollisionPositionChange(collisionIndex, newPosition, collidedMesh);
    },
    uniqueId);
}

void FreeCamera::_onCollisionPositionChange(size_t /*collisionId*/,
                                            Vector3& newPosition,
                                            const AbstractMeshPtr& collidedMesh)
{
  auto updatePosition = [&](Vector3& newPos) {
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

void FreeCamera::_checkInputs()
{
  if (!_localDirection) {
    _localDirection       = std::make_unique<Vector3>(Vector3::Zero());
    _transformedDirection = Vector3::Zero();
  }

  inputs->checkInputs();

  TargetCamera::_checkInputs();
}

bool FreeCamera::_decideIfNeedsToMove()
{
  return _needMoveForGravity || std::abs(cameraDirection->x) > 0.f
         || std::abs(cameraDirection->y) > 0.f
         || std::abs(cameraDirection->z) > 0.f;
}

void FreeCamera::_updatePosition()
{
  if (checkCollisions && getScene()->collisionsEnabled) {
    _collideWithWorld(*cameraDirection);
  }
  else {
    TargetCamera::_updatePosition();
  }
}

void FreeCamera::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  inputs->clear();
  TargetCamera::dispose(doNotRecurse, disposeMaterialAndTextures);
}

std::string FreeCamera::getClassName() const
{
  return "FreeCamera";
}

json FreeCamera::serialize() const
{
  return json();
}

} // end of namespace BABYLON
