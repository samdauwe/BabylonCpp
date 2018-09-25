#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_SPATIAL_PARTITION_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_SPATIAL_PARTITION_H

#include <babylon/babylon_api.h>
#include <babylon/culling/bounding_sphere.h>

namespace BABYLON {

class Ray;

namespace Extensions {

struct SpatialPartition;
struct Tile;

struct SpatialPartition {
  SpatialPartition();
  SpatialPartition(const BoundingSphere& p_boundingSphere,
                   const std::vector<SpatialPartition>& p_partitions,
                   const std::vector<Tile*>& p_tiles);
  bool intersectRay(const Ray& ray);

  BoundingSphere boundingSphere;
  std::vector<SpatialPartition> partitions;
  std::vector<Tile*> tiles;
}; // end of struct SpatialPartition

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_SPATIAL_PARTITION_H
