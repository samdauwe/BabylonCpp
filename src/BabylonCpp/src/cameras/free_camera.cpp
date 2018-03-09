#include <babylon/cameras/free_camera.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/collider.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/core/json.h>
#include <babylon/core/string.h>
#include <babylon/engine/engine.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

FreeCamera::FreeCamera(const string_t& iName, const Vector3& iPosition,
                       Scene* scene)
    : TargetCamera{iName, iPosition, scene}
    , ellipsoid{Vector3(0.5f, 1.f, 0.5f)}
    , ellipsoidOffset{Vector3(0.f, 0.f, 0.f)}
    , checkCollisions{false}
    , applyGravity{false}
    , inputs{::std::make_unique<FreeCameraInputsManager>(this)}
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

  cameraDirection = ::std::make_unique<Vector3>(0.f, 0.f, 0.f);
  cameraRotation  = ::std::make_unique<Vector2>(0.f, 0.f);
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
      = Vector3::TransformCoordinates(position, *parent()->getWorldMatrix());
  }
  else {
    globalPosition_ = position;
  }

  globalPosition_.subtractFromFloatsToRef(0, ellipsoid.y, 0, _oldPosition);
  _oldPosition.addInPlace(ellipsoidOffset);

  if (!_collider) {
    _collider = ::std::make_unique<Collider>();
  }

  _collider->_radius = ellipsoid;
  _collider->setCollisionMask(_collisionMask);

  // no need for clone, as long as gravity is not on.
  auto actualDisplacement = displacement;

  // add gravity to the direction to prevent the dual-collision checking
  if (applyGravity) {
    // this prevents mending with cameraDirection, a global variable of the free
    // camera class.
    actualDisplacement = displacement.add(getScene()->gravity);
  }

  getScene()->collisionCoordinator->getNewPosition(
    _oldPosition, actualDisplacement, _collider.get(), 3, nullptr,
    [&](int collisionId, Vector3& newPosition, AbstractMesh* collidedMesh) {
      _onCollisionPositionChange(collisionId, newPosition, collidedMesh);
    },
    static_cast<unsigned>(uniqueId));
}

void FreeCamera::_onCollisionPositionChange(int /*collisionId*/,
                                            Vector3& newPosition,
                                            AbstractMesh* collidedMesh)
{
  if (getScene()->workerCollisions()) {
    newPosition.multiplyInPlace(_collider->_radius);
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
    _localDirection       = ::std::make_unique<Vector3>(Vector3::Zero());
    _transformedDirection = Vector3::Zero();
  }

  inputs->checkInputs();

  TargetCamera::_checkInputs();
}

bool FreeCamera::_decideIfNeedsToMove()
{
  return _needMoveForGravity || ::std::abs(cameraDirection->x) > 0.f
         || ::std::abs(cameraDirection->y) > 0.f
         || ::std::abs(cameraDirection->z) > 0.f;
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
