#ifndef BABYLON_COLLISIONS_COLLIDE_WORKER_H
#define BABYLON_COLLISIONS_COLLIDE_WORKER_H

#include <babylon/babylon_api.h>
#include <babylon/collisions/collision_cache.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Collider;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT CollideWorker {

public:
  CollideWorker(Collider* collider, const CollisionCache& collisionCache,
                const Vector3& finalPosition);
  ~CollideWorker();

  void collideWithWorld(Vector3& position, Vector3& velocity,
                        unsigned int maximumRetry, int excludedMeshUniqueId);

private:
  void checkCollision(SerializedMesh& mesh);
  void processCollisionsForSubMeshes(const Matrix& transformMatrix,
                                     SerializedMesh& mesh);
  void collideForSubMesh(SerializedSubMesh& subMesh,
                         const Matrix& transformMatrix,
                         SerializedGeometry& meshGeometry);
  bool checkSubmeshCollision(const SerializedSubMesh& subMesh);

public:
  Collider* collider;

private:
  Matrix collisionsScalingMatrix;
  Matrix collisionTranformationMatrix;
  CollisionCache _collisionCache;
  Vector3 finalPosition;

}; // end of class CollidePayload

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLIDE_WORKER_H
