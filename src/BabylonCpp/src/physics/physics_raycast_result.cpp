#include <babylon/physics/physics_raycast_result.h>

namespace BABYLON {

PhysicsRaycastResult::PhysicsRaycastResult()
    : hasHit{this, &PhysicsRaycastResult::get_hasHit}
    , hitDistance{this, &PhysicsRaycastResult::get_hitDistance}
    , hitNormalWorld{this, &PhysicsRaycastResult::get_hitNormalWorld}
    , hitPointWorld{this, &PhysicsRaycastResult::get_hitPointWorld}
    , rayFromWorld{this, &PhysicsRaycastResult::get_rayFromWorld}
    , rayToWorld{this, &PhysicsRaycastResult::get_rayToWorld}
    , _hasHit{false}
    , _hitDistance{0.f}
    , _hitNormalWorld{Vector3::Zero()}
    , _hitPointWorld{Vector3::Zero()}
    , _rayFromWorld{Vector3::Zero()}
    , _rayToWorld{Vector3::Zero()}
{
}

PhysicsRaycastResult::PhysicsRaycastResult(const PhysicsRaycastResult& rhs)
    : hasHit{this, &PhysicsRaycastResult::get_hasHit}
    , hitDistance{this, &PhysicsRaycastResult::get_hitDistance}
    , hitNormalWorld{this, &PhysicsRaycastResult::get_hitNormalWorld}
    , hitPointWorld{this, &PhysicsRaycastResult::get_hitPointWorld}
    , rayFromWorld{this, &PhysicsRaycastResult::get_rayFromWorld}
    , rayToWorld{this, &PhysicsRaycastResult::get_rayToWorld}
    , _hasHit{rhs._hasHit}
    , _hitDistance{rhs._hitDistance}
    , _hitNormalWorld{rhs._hitNormalWorld}
    , _hitPointWorld{rhs._hitPointWorld}
    , _rayFromWorld{rhs._rayFromWorld}
    , _rayToWorld{rhs._rayToWorld}
{
}

PhysicsRaycastResult::PhysicsRaycastResult(PhysicsRaycastResult&& rhs)
    : hasHit{this, &PhysicsRaycastResult::get_hasHit}
    , hitDistance{this, &PhysicsRaycastResult::get_hitDistance}
    , hitNormalWorld{this, &PhysicsRaycastResult::get_hitNormalWorld}
    , hitPointWorld{this, &PhysicsRaycastResult::get_hitPointWorld}
    , rayFromWorld{this, &PhysicsRaycastResult::get_rayFromWorld}
    , rayToWorld{this, &PhysicsRaycastResult::get_rayToWorld}
    , _hasHit{std::move(rhs._hasHit)}
    , _hitDistance{std::move(rhs._hitDistance)}
    , _hitNormalWorld{std::move(rhs._hitNormalWorld)}
    , _hitPointWorld{std::move(rhs._hitPointWorld)}
    , _rayFromWorld{std::move(rhs._rayFromWorld)}
    , _rayToWorld{std::move(rhs._rayToWorld)}
{
}

PhysicsRaycastResult& PhysicsRaycastResult::
operator=(const PhysicsRaycastResult& rhs)
{
  if (&rhs != this) {
    _hasHit         = rhs._hasHit;
    _hitDistance    = rhs._hitDistance;
    _hitNormalWorld = rhs._hitNormalWorld;
    _hitPointWorld  = rhs._hitPointWorld;
    _rayFromWorld   = rhs._rayFromWorld;
    _rayToWorld     = rhs._rayToWorld;
  }

  return *this;
}

PhysicsRaycastResult& PhysicsRaycastResult::
operator=(PhysicsRaycastResult&& rhs)
{
  if (&rhs != this) {
    _hasHit         = std::move(rhs._hasHit);
    _hitDistance    = std::move(rhs._hitDistance);
    _hitNormalWorld = std::move(rhs._hitNormalWorld);
    _hitPointWorld  = std::move(rhs._hitPointWorld);
    _rayFromWorld   = std::move(rhs._rayFromWorld);
    _rayToWorld     = std::move(rhs._rayToWorld);
  }

  return *this;
}

PhysicsRaycastResult::~PhysicsRaycastResult()
{
}

bool PhysicsRaycastResult::get_hasHit() const
{
  return _hasHit;
}

float PhysicsRaycastResult::get_hitDistance() const
{
  return _hitDistance;
}

Vector3& PhysicsRaycastResult::get_hitNormalWorld()
{
  return _hitNormalWorld;
}

Vector3& PhysicsRaycastResult::get_hitPointWorld()
{
  return _hitPointWorld;
}

Vector3& PhysicsRaycastResult::get_rayFromWorld()
{
  return _rayFromWorld;
}

Vector3& PhysicsRaycastResult::get_rayToWorld()
{
  return _rayToWorld;
}

void PhysicsRaycastResult::setHitData(const IXYZ& hitNormalWorld,
                                      const IXYZ& hitPointWorld)
{
  _hasHit = true;
  _hitNormalWorld
    = Vector3(hitNormalWorld.x, hitNormalWorld.y, hitNormalWorld.z);
  _hitPointWorld = Vector3(hitPointWorld.x, hitPointWorld.y, hitPointWorld.z);
}

void PhysicsRaycastResult::setHitDistance(float distance)
{
  _hitDistance = distance;
}

void PhysicsRaycastResult::calculateHitDistance()
{
  _hitDistance = Vector3::Distance(_rayFromWorld, _hitPointWorld);
}

void PhysicsRaycastResult::reset(const Vector3& from, const Vector3& to)
{
  _rayFromWorld = from;
  _rayToWorld   = to;

  _hasHit      = false;
  _hitDistance = 0;

  _hitNormalWorld = Vector3::Zero();
  _hitPointWorld  = Vector3::Zero();
}

} // end of namespace BABYLON
