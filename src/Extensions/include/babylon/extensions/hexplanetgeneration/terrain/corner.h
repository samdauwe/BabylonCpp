#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_CORNER_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_CORNER_H

#include <babylon/babylon_global.h>

#include <babylon/extensions/hexplanetgeneration/terrain/uuid_obj.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct Corner;
struct Border;
struct Tile;

struct Corner : public UidObj {
  Corner(size_t p_id, const Vector3& p_position, size_t cornerCount,
         size_t borderCount, size_t tileCount);
  Vector3 vectorTo(const Corner& corner);
  std::string toString();

  Vector3 position;
  float area;
  std::vector<Corner*> corners;
  std::vector<Border*> borders;
  std::vector<Tile*> tiles;
  float distanceToPlateRoot;
  float distanceToPlateBoundary;
  bool betweenPlates;
  float pressure;
  float shear;
  float elevation;
  Vector3 airCurrent;
  float airCurrentSpeed; // kilometers per hour
  Float32Array airCurrentOutflows;
  // AirHeat
  float temperature;
  // 2do: temporaries for temperature calc. ?!
  float airHeat;
  float newAirHeat;
  float heat;
  float heatAbsorption;
  float maxHeat;
  // AirMoisture
  float moisture;
  // 2do: temporaries for temperature calc. ?!
  float airMoisture;
  float newAirMoisture;
  float precipitation;
  float precipitationRate;
  float maxPrecipitation;
}; // end of struct Corner

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_CORNER_H
