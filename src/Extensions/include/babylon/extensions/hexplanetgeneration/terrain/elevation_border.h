#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_ELEVATION_BORDER_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_ELEVATION_BORDER_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/hexplanetgeneration/terrain/elevation_border_origin.h>

namespace BABYLON {
namespace Extensions {

struct Border;
struct Corner;

struct ElevationBorder {
  ElevationBorder();
  ElevationBorder(ElevationBorderOrigin& p_origin, Border* p_border,
                  Corner* p_corner, Corner* p_nextCorner,
                  float p_distanceToPlateBoundary);

  ElevationBorderOrigin origin;
  Border* border;
  Corner* corner;
  Corner* nextCorner;
  float distanceToPlateBoundary;
};

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
// BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_ELEVATION_BORDER_H
