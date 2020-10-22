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
    , _collisionResponse{true}
{
}

_MeshCollisionData::_MeshCollisionData(const _MeshCollisionData& other) = default;

_MeshCollisionData::_MeshCollisionData(_MeshCollisionData&& other) = default;

_MeshCollisionData& _MeshCollisionData::operator=(const _MeshCollisionData& other) = default;

_MeshCollisionData& _MeshCollisionData::operator=(_MeshCollisionData&& other) = default;

_MeshCollisionData::~_MeshCollisionData() = default;

} // end of namespace BABYLON
