#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_BORDER_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_BORDER_H

#include <babylon/babylon_api.h>
#include <babylon/extensions/hexplanetgeneration/terrain/uuid_obj.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct Corner;
struct Tile;

struct Border : public UidObj {
  Border(size_t p_id, size_t cornerCount, size_t borderCount, size_t tileCount);

  Corner& oppositeCorner(const Corner& corner);
  [[nodiscard]] const Corner& oppositeCorner(const Corner& corner) const;
  Tile& oppositeTile(const Tile& tile);
  [[nodiscard]] const Tile& oppositeTile(const Tile& tile) const;
  [[nodiscard]] float length() const;
  [[nodiscard]] bool isLandBoundary() const;
  [[nodiscard]] std::string toString() const;

  std::vector<Corner*> corners;
  std::vector<Border*> borders;
  std::vector<Tile*> tiles;
  Vector3 midpoint;
  bool betweenPlates;
}; // end of struct Border

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_BORDER_H
