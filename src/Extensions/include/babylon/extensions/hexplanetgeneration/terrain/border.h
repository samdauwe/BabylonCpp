#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_BORDER_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_BORDER_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/hexplanetgeneration/terrain/uuid_obj.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct Corner;
struct Tile;

struct Border : public UidObj {
  Border(size_t p_id, size_t cornerCount, size_t borderCount, size_t tileCount);

  Corner& oppositeCorner(const Corner& corner);
  const Corner& oppositeCorner(const Corner& corner) const;
  Tile& oppositeTile(const Tile& tile);
  const Tile& oppositeTile(const Tile& tile) const;
  float length() const;
  bool isLandBoundary() const;
  std::string toString() const;

  std::vector<Corner*> corners;
  std::vector<Border*> borders;
  std::vector<Tile*> tiles;
  Vector3 midpoint;
  bool betweenPlates;
};

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_BORDER_H
