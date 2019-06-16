#ifndef BABYLON_COLLISIONS_MESH_COLLISION_DATA_H
#define BABYLON_COLLISIONS_MESH_COLLISION_DATA_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/vector3.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractMesh;
class Collider;
using ColliderPtr = std::shared_ptr<Collider>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT _MeshCollisionData {

public:
  _MeshCollisionData();
  _MeshCollisionData(const _MeshCollisionData& other);
  _MeshCollisionData(_MeshCollisionData&& other);
  _MeshCollisionData& operator=(const _MeshCollisionData& other);
  _MeshCollisionData& operator=(_MeshCollisionData&& other);
  ~_MeshCollisionData();

public:
  bool _checkCollisions;
  int _collisionMask;
  int _collisionGroup;
  ColliderPtr _collider;
  Vector3 _oldPositionForCollisions;
  Vector3 _diffPositionForCollisions;
  Observer<AbstractMesh>::Ptr _onCollideObserver;
  Observer<Vector3>::Ptr _onCollisionPositionChangeObserver;

}; // end of class _MeshCollisionData

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_MESH_COLLISION_DATA_H
