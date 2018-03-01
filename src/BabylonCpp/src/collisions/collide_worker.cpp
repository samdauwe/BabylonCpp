#include <babylon/collisions/collide_worker.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/collisions/collider.h>
#include <babylon/core/logging.h>
#include <babylon/math/plane.h>

namespace BABYLON {

CollideWorker::CollideWorker(Collider* _collider,
                             const CollisionCache& collisionCache,
                             const Vector3& _finalPosition)
    : collider{_collider}
    , collisionsScalingMatrix{Matrix::Zero()}
    , collisionTranformationMatrix{Matrix::Zero()}
    , _collisionCache{collisionCache}
    , finalPosition{_finalPosition}
{
}

CollideWorker::~CollideWorker()
{
}

void CollideWorker::collideWithWorld(Vector3& position, Vector3& velocity,
                                     unsigned int maximumRetry,
                                     int excludedMeshUniqueId)
{

  const float closeDistance = 0.01f;
  if (collider->_retry >= maximumRetry) {
    finalPosition.copyFrom(position);
    return;
  }

  collider->_initialize(position, velocity, closeDistance);

  // Check all meshes
  unordered_map_t<unsigned int, SerializedMesh>& meshes
    = _collisionCache.getMeshes();

  if (excludedMeshUniqueId >= 0) {
    unsigned _excludedMeshUniqueId
      = static_cast<unsigned>(excludedMeshUniqueId);
    for (auto& item : meshes) {
      if (item.first != _excludedMeshUniqueId) {
        SerializedMesh& mesh = item.second;
        if (mesh.checkCollisions) {
          checkCollision(mesh);
        }
      }
    }
  }

  if (!collider->collisionFound) {
    position.addToRef(velocity, finalPosition);
    return;
  }

  if (!stl_util::almost_equal(velocity.x, 0.f)
      || !stl_util::almost_equal(velocity.y, 0.f)
      || !stl_util::almost_equal(velocity.z, 0.f)) {
    collider->_getResponse(position, velocity);
  }

  if (velocity.length() <= closeDistance) {
    finalPosition.copyFrom(position);
    return;
  }

  ++collider->_retry;
  collideWithWorld(position, velocity, maximumRetry, excludedMeshUniqueId);
}

void CollideWorker::checkCollision(SerializedMesh& mesh)
{
  if (!collider->_canDoCollision(Vector3::FromArray(mesh.sphereCenter),
                                 mesh.sphereRadius,
                                 Vector3::FromArray(mesh.boxMinimum),
                                 Vector3::FromArray(mesh.boxMaximum))) {
    return;
  };

  // Transformation matrix
  Matrix::ScalingToRef(1.f / collider->_radius.x, 1.f / collider->_radius.y,
                       1.f / collider->_radius.z, collisionsScalingMatrix);
  Matrix worldFromCache = Matrix::FromArray(mesh.worldMatrixFromCache);
  worldFromCache.multiplyToRef(collisionsScalingMatrix,
                               collisionTranformationMatrix);

  processCollisionsForSubMeshes(collisionTranformationMatrix, mesh);
}

void CollideWorker::processCollisionsForSubMeshes(const Matrix& transformMatrix,
                                                  SerializedMesh& mesh)
{
  vector_t<SerializedSubMesh>& subMeshes = mesh.subMeshes;
  size_t len                             = subMeshes.size();

  if (mesh.geometryId.empty()) {
    BABYLON_LOG_ERROR("CollideWorker", "no mesh geometry id");
    return;
  }

  if (!_collisionCache.containsGeometry(mesh.geometryId)) {
    BABYLON_LOG_ERROR("CollideWorker",
                      "couldn't find geometry " + mesh.geometryId);
    return;
  }

  SerializedGeometry& meshGeometry
    = _collisionCache.getGeometry(mesh.geometryId);
  for (auto& subMesh : subMeshes) {
    // Bounding test
    if (len > 1 && !checkSubmeshCollision(subMesh)) {
      continue;
    }

    collideForSubMesh(subMesh, transformMatrix, meshGeometry);
    if (collider->collisionFound) {
      collider->collidedMeshId = mesh.uniqueId;
    }
  }
}

void CollideWorker::collideForSubMesh(SerializedSubMesh& subMesh,
                                      const Matrix& transformMatrix,
                                      SerializedGeometry& meshGeometry)
{
  if (meshGeometry.positionsArray.empty()) {
    for (size_t i = 0; i < meshGeometry.positions.size(); i = i + 3) {
      auto p = Vector3::FromArray({meshGeometry.positions[i],
                                   meshGeometry.positions[i + 1],
                                   meshGeometry.positions[i + 2]});
      meshGeometry.positionsArray.emplace_back(p);
    }
  }

  if (subMesh._lastColliderWorldVertices.empty()
      || subMesh._lastColliderTransformMatrix.equals(transformMatrix)) {
    subMesh._lastColliderTransformMatrix = transformMatrix;
    subMesh._lastColliderWorldVertices.clear();
    subMesh._trianglePlanes.clear();
    auto start = subMesh.verticesStart;
    auto end   = (subMesh.verticesStart + subMesh.verticesCount);
    for (size_t i = start; i < end; ++i) {
      subMesh._lastColliderWorldVertices.emplace_back(
        Vector3::TransformCoordinates(meshGeometry.positionsArray[i],
                                      transformMatrix));
    }
  }

  // Collide
  collider->_collide(subMesh._trianglePlanes,
                     subMesh._lastColliderWorldVertices, meshGeometry.indices,
                     subMesh.indexStart,
                     subMesh.indexStart + subMesh.indexCount,
                     subMesh.verticesStart, subMesh.hasMaterial);
}

bool CollideWorker::checkSubmeshCollision(const SerializedSubMesh& subMesh)
{
  return collider->_canDoCollision(Vector3::FromArray(subMesh.sphereCenter),
                                   subMesh.sphereRadius,
                                   Vector3::FromArray(subMesh.boxMinimum),
                                   Vector3::FromArray(subMesh.boxMaximum));
}

} // end of namespace BABYLON
