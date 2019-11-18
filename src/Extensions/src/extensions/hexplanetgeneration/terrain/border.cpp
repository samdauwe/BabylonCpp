#include <babylon/extensions/hexplanetgeneration/terrain/border.h>

#include <sstream>

#include <babylon/extensions/hexplanetgeneration/terrain/corner.h>
#include <babylon/extensions/hexplanetgeneration/terrain/tile.h>

namespace BABYLON {
namespace Extensions {

Border::Border(size_t p_id, size_t cornerCount, size_t borderCount, size_t tileCount)
    : UidObj{p_id}, betweenPlates{false}
{
  corners.resize(cornerCount);
  borders.resize(borderCount);
  tiles.resize(tileCount);
}

Corner& Border::oppositeCorner(const Corner& corner)
{
  return (*corners[0] == corner) ? *corners[1] : *corners[0];
}

const Corner& Border::oppositeCorner(const Corner& corner) const
{
  return (*corners[0] == corner) ? *corners[1] : *corners[0];
}

Tile& Border::oppositeTile(const Tile& tile)
{
  return (*tiles[0] == tile) ? *tiles[1] : *tiles[0];
}

const Tile& Border::oppositeTile(const Tile& tile) const
{
  return (*tiles[0] == tile) ? *tiles[1] : *tiles[0];
}

float Border::length() const
{
  return Vector3::Distance(corners[0]->position, corners[1]->position);
}

bool Border::isLandBoundary() const
{
  return (tiles[0]->elevation > 0) != (tiles[1]->elevation > 0);
}

std::string Border::toString() const
{
  std::stringstream ss;
  ss << "Border " << id;
  return ss.str();
}

} // end of namespace Extensions
} // end of namespace BABYLON
