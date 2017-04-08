#include <babylon/cameras/free_camera.h>

#include <babylon/collisions/collider.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/core/json.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

FreeCamera::FreeCamera(const std::string& iName, const Vector3& iPosition,
                       Scene* scene)
    : TargetCamera{iName, iPosition, scene}
    , ellipsoid{Vector3(0.5f, 1.f, 0.5f)}
    , checkCollisions{false}
    , applyGravity{false}
    , inputs{std_util::make_unique<FreeCameraInputsManager>(this)}
    , _collider{std_util::make_unique<Collider>()}
    , _needMoveForGravity{false}
    , _oldPosition{Vector3::Zero()}
    , _diffPosition{Vector3::Zero()}
    , _newPosition{Vector3::Zero()}
{
  inputs->addKeyboard().addMouse();
}

FreeCamera::~FreeCamera()
{
}

IReflect::Type FreeCamera::type() const
{
  return IReflect::Type::FREECAMERA;
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

  cameraDirection = std_util::make_unique<Vector3>(0.f, 0.f, 0.f);
  cameraRotation  = std_util::make_unique<Vector2>(0.f, 0.f);
}

void FreeCamera::_collideWithWorld(Vector3& velocity)
{
  Vector3 globalPosition_;

  if (parent()) {
    globalPosition_
      = Vector3::TransformCoordinates(position, *parent()->getWorldMatrix());
  }
  else {
    globalPosition_ = position;
  }

  globalPosition_.subtractFromFloatsToRef(0, ellipsoid.y, 0, _oldPosition);
  _collider->radius = ellipsoid;

  // no need for clone, as long as gravity is not on.
  auto actualVelocity = velocity;

  // add gravity to the velocity to prevent the dual-collision checking
  if (applyGravity) {
    // this prevents mending with cameraDirection, a global variable of the free
    // camera class.
    actualVelocity = velocity.add(getScene()->gravity);
  }

  getScene()->collisionCoordinator->getNewPosition(
    _oldPosition, actualVelocity, _collider.get(), 3, nullptr,
    [&](int collisionId, Vector3& newPosition, AbstractMesh* collidedMesh) {
      _onCollisionPositionChange(collisionId, newPosition, collidedMesh);
    },
    uniqueId);
}

void FreeCamera::_onCollisionPositionChange(int /*collisionId*/,
                                            Vector3& newPosition,
                                            AbstractMesh* collidedMesh)
{
  if (getScene()->workerCollisions()) {
    newPosition.multiplyInPlace(_collider->radius);
  }

  auto updatePosition = [&](Vector3& newPos) {
    _newPosition.copyFrom(newPos);

    _newPosition.subtractToRef(_oldPosition, _diffPosition);

    if (_diffPosition.length() > Engine::CollisionsEpsilon) {
      position.addInPlace(_diffPosition);
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
    _localDirection       = std_util::make_unique<Vector3>(Vector3::Zero());
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
    position.addInPlace(*cameraDirection);
  }
}

void FreeCamera::dispose(bool doNotRecurse)
{
  inputs->clear();
  TargetCamera::dispose(doNotRecurse);
}

const char* FreeCamera::getClassName() const
{
  return "FreeCamera";
}

Json::object FreeCamera::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
