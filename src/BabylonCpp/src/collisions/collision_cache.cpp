#include <babylon/collisions/collision_cache.h>

#include <babylon/babylon_stl_util.h>

namespace BABYLON {

CollisionCache::CollisionCache()
{
}

CollisionCache::~CollisionCache()
{
}

std::unordered_map<unsigned int, SerializedMesh>& CollisionCache::getMeshes()
{
  return _meshes;
}

std::unordered_map<std::string, SerializedGeometry>&
CollisionCache::getGeometries()
{
  return _geometries;
}

bool CollisionCache::containsMesh(unsigned int id) const
{
  return _meshes.find(id) != _meshes.end();
}

SerializedMesh& CollisionCache::getMesh(unsigned int id)
{
  return _meshes[id];
}

void CollisionCache::addMesh(const SerializedMesh& mesh)
{
  _meshes[mesh.uniqueId] = mesh;
}

void CollisionCache::removeMesh(unsigned int uniqueId)
{
  _meshes.erase(uniqueId);
}

bool CollisionCache::containsGeometry(const std::string& id) const
{
  return stl_util::contains(_geometries, id);
}

SerializedGeometry& CollisionCache::getGeometry(const std::string& id)
{
  return _geometries[id];
}

void CollisionCache::addGeometry(const SerializedGeometry& geometry)
{
  _geometries[geometry.id] = geometry;
}

void CollisionCache::removeGeometry(const std::string& id)
{
  _geometries.erase(id);
}

} // end of namespace BABYLON
