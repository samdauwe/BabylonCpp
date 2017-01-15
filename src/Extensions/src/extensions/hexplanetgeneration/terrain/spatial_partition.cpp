#include <babylon/extensions/hexplanetgeneration/terrain/spatial_partition.h>

#include <babylon/extensions/hexplanetgeneration/terrain/tile.h>
#include <babylon/extensions/hexplanetgeneration/utils/tools.h>

namespace BABYLON {
namespace Extensions {

SpatialPartition::SpatialPartition()
    : boundingSphere{Vector3::Zero(), Vector3::Zero()}, partitions{}, tiles{}
{
}

SpatialPartition::SpatialPartition(
  const BoundingSphere& p_boundingSphere,
  const std::vector<SpatialPartition>& p_partitions,
  const std::vector<Tile*>& p_tiles)
    : boundingSphere{p_boundingSphere}, partitions{p_partitions}, tiles{p_tiles}
{
}

bool SpatialPartition::intersectRay(const Ray& ray)
{
  if (Tools::intersectRayWithSphere(ray, boundingSphere)) {
    for (auto& partition : partitions) {
      auto intersection = partition.intersectRay(ray);
      if (intersection != false) {
        return true;
      }
    }

    for (auto& tile : tiles) {
      if (tile->intersectRay(ray)) {
        return true;
      }
    }
  }

  return false;
}

} // end of namespace Extensions
} // end of namespace BABYLON
