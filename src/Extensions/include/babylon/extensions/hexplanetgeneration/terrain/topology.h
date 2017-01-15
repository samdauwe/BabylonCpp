#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TOPOLOGY_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TOPOLOGY_H

#include <babylon/babylon_global.h>

#include <babylon/extensions/hexplanetgeneration/terrain/corner.h>
#include <babylon/extensions/hexplanetgeneration/terrain/border.h>
#include <babylon/extensions/hexplanetgeneration/terrain/tile.h>

namespace BABYLON {
namespace Extensions {

struct Corner;
struct Tile;

struct Topology {
  std::vector<Corner> corners;
  std::vector<Border> borders;
  std::vector<Tile> tiles;
};

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TOPOLOGY_H
