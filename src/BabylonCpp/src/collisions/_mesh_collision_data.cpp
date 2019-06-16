#include <babylon/collisions/_mesh_collision_data.h>

namespace BABYLON {

_MeshCollisionData::_MeshCollisionData()
    : _checkCollisions{false}
    , _collisionMask{-1}
    , _collisionGroup{-1}
    , _collider{nullptr}
    , _oldPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _diffPositionForCollisions{Vector3(0.f, 0.f, 0.f)}
    , _onCollideObserver{nullptr}
    , _onCollisionPositionChangeObserver{nullptr}
{
}

_MeshCollisionData::_MeshCollisionData(const _MeshCollisionData& other)
    : _checkCollisions{other._checkCollisions}
    , _collisionMask{other._collisionMask}
    , _collisionGroup{other._collisionGroup}
    , _collider{other._collider}
    , _oldPositionForCollisions{other._oldPositionForCollisions}
    , _diffPositionForCollisions{other._diffPositionForCollisions}
    , _onCollideObserver{other._onCollideObserver}
    , _onCollisionPositionChangeObserver{
        other._onCollisionPositionChangeObserver}
{
}

_MeshCollisionData::_MeshCollisionData(_MeshCollisionData&& other)
    : _checkCollisions{std::move(other._checkCollisions)}
    , _collisionMask{std::move(other._collisionMask)}
    , _collisionGroup{std::move(other._collisionGroup)}
    , _collider{std::move(other._collider)}
    , _oldPositionForCollisions{std::move(other._oldPositionForCollisions)}
    , _diffPositionForCollisions{std::move(other._diffPositionForCollisions)}
    , _onCollideObserver{std::move(other._onCollideObserver)}
    , _onCollisionPositionChangeObserver{
        std::move(other._onCollisionPositionChangeObserver)}
{
}

_MeshCollisionData& _MeshCollisionData::
operator=(const _MeshCollisionData& other)
{
  if (&other != this) {
    _checkCollisions           = other._checkCollisions;
    _collisionMask             = other._collisionMask;
    _collisionGroup            = other._collisionGroup;
    _collider                  = other._collider;
    _oldPositionForCollisions  = other._oldPositionForCollisions;
    _diffPositionForCollisions = other._diffPositionForCollisions;
    _onCollideObserver         = other._onCollideObserver;
    _onCollisionPositionChangeObserver
      = other._onCollisionPositionChangeObserver;
  }

  return *this;
}

_MeshCollisionData& _MeshCollisionData::operator=(_MeshCollisionData&& other)
{
  if (&other != this) {
    _checkCollisions           = std::move(other._checkCollisions);
    _collisionMask             = std::move(other._collisionMask);
    _collisionGroup            = std::move(other._collisionGroup);
    _collider                  = std::move(other._collider);
    _oldPositionForCollisions  = std::move(other._oldPositionForCollisions);
    _diffPositionForCollisions = std::move(other._diffPositionForCollisions);
    _onCollideObserver         = std::move(other._onCollideObserver);
    _onCollisionPositionChangeObserver
      = std::move(other._onCollisionPositionChangeObserver);
  }

  return *this;
}

_MeshCollisionData::~_MeshCollisionData()
{
}

} // end of namespace BABYLON
