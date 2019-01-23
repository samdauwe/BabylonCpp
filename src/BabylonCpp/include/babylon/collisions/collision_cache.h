#ifndef BABYLON_COLLISIONS_COLLISION_CACHE_H
#define BABYLON_COLLISIONS_COLLISION_CACHE_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/collisions/serialized_geometry.h>
#include <babylon/collisions/serialized_mesh.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT CollisionCache {

public:
  CollisionCache();
  ~CollisionCache();

  std::unordered_map<size_t, SerializedMesh>& getMeshes();
  std::unordered_map<std::string, SerializedGeometry>& getGeometries();
  bool containsMesh(unsigned int id) const;
  SerializedMesh& getMesh(unsigned int id);
  void addMesh(const SerializedMesh& mesh);
  void removeMesh(size_t uniqueId);
  bool containsGeometry(const std::string& id) const;
  SerializedGeometry& getGeometry(const std::string& id);
  void addGeometry(const SerializedGeometry& geometry);
  void removeGeometry(const std::string& id);

private:
  std::unordered_map<size_t, SerializedMesh> _meshes;
  std::unordered_map<std::string, SerializedGeometry> _geometries;

}; // end of class CollisionCache

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_CACHE_H
