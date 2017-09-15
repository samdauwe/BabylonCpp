#ifndef BABYLON_COLLISIONS_COLLISION_CACHE_H
#define BABYLON_COLLISIONS_COLLISION_CACHE_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/serialized_geometry.h>
#include <babylon/collisions/serialized_mesh.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CollisionCache {

public:
  CollisionCache();
  ~CollisionCache();

  unordered_map_t<unsigned int, SerializedMesh>& getMeshes();
  unordered_map_t<string_t, SerializedGeometry>& getGeometries();
  bool containsMesh(unsigned int id) const;
  SerializedMesh& getMesh(unsigned int id);
  void addMesh(const SerializedMesh& mesh);
  void removeMesh(unsigned int uniqueId);
  bool containsGeometry(const string_t& id) const;
  SerializedGeometry& getGeometry(const string_t& id);
  void addGeometry(const SerializedGeometry& geometry);
  void removeGeometry(const string_t& id);

private:
  unordered_map_t<unsigned int, SerializedMesh> _meshes;
  unordered_map_t<string_t, SerializedGeometry> _geometries;

}; // end of class CollisionCache

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLISION_CACHE_H
