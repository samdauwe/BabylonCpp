#include <babylon/extensions/hexplanetgeneration/terrain/tile.h>

namespace BABYLON {
namespace Extensions {

Tile::Tile(size_t p_id, const Vector3& p_position, size_t cornerCount,
           size_t borderCount, size_t tileCount)
    : UidObj{p_id}
    , position{p_position}
    , boundingSphere{Vector3::Zero(), Vector3::Zero()}
    , normal{}
    , averagePosition{}
    , area{0}
    , elevation{0}
    , corners{}
    , borders{}
    , tiles{}
    , plate{nullptr}
    , temperature{0}
    , moisture{0}
    , biome{}
    , plateMovement()
{
  corners.resize(cornerCount);
  borders.resize(borderCount);
  tiles.resize(tileCount);
}

bool Tile::intersectRay(const Ray& /*ray*/)
{
  return true;
}

std::string Tile::toString()
{
  std::ostringstream oss;
  oss << "Tile " << id << " (" << tiles.size() << " Neighbors) < " << position.x
      << ", " << position.y << ", " << position.z << " >";
  return oss.str();
}

} // end of namespace Extensions
} // end of namespace BABYLON
