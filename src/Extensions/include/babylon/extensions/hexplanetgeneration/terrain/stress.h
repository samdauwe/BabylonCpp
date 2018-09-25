#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_STRESS_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_STRESS_H

#include <babylon/babylon_api.h>

namespace BABYLON {
namespace Extensions {

struct Stress {
  Stress();
  Stress(float p_pressure, float p_shear);

  float pressure;
  float shear;
}; // end of struct Stress

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_STRESS_H
