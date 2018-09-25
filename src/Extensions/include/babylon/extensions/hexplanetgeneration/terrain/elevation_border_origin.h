#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_ELEVATION_BORDER_ORIGIN_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_ELEVATION_BORDER_ORIGIN_H

#include <functional>

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct Corner;
struct Plate;

struct ElevationBorderOrigin {
  typedef std::function<float(float, float, float, float, float, float)>
    CalculateElevationFunc;

  ElevationBorderOrigin();
  ElevationBorderOrigin(Corner* p_corner, float p_pressure, float p_shear,
                        Plate* p_plate,
                        CalculateElevationFunc& p_calculateElevation);

  Corner* corner;
  float pressure;
  float shear;
  Plate* plate;
  CalculateElevationFunc calculateElevation;
}; // end of struct ElevationBorderOrigin

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
// BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_ELEVATION_BORDER_ORIGIN_H
