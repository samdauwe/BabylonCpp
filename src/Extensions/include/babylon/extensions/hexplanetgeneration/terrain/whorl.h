#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_WHORL_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_WHORL_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct Whorl {
  Whorl();
  Whorl(Vector3&& p_center, float p_strength, float p_radius);

  Vector3 center;
  float strength;
  float radius;
}; // end of struct Whorl

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_WHORL_H
