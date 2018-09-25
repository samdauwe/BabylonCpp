#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_PLATE_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_PLATE_H

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct Border;
struct Corner;
struct Tile;

struct Plate {
  Plate(const Color3& p_color, const Vector3& p_driftAxis, float p_driftRate,
        float p_spinRate, float p_elevation, bool p_oceanic, Corner& p_root);
  Vector3 calculateMovement(const Vector3& position);

  Color3 color;
  Vector3 driftAxis;
  float driftRate;
  float spinRate;
  float elevation;
  bool oceanic;
  Corner* root;
  std::vector<Tile*> tiles;
  std::vector<Corner*> boundaryCorners;
  std::vector<Border*> boundaryBorders;
  float area;
  float circumference;
}; // end of struct Plate

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_PLATE_H
