#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TILE_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TILE_H

#include <babylon/babylon_global.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/extensions/hexplanetgeneration/terrain/uuid_obj.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

struct Border;
struct Corner;
struct Plate;

struct Tile : public UidObj {
  Tile(size_t p_id, const Vector3& p_position, size_t cornerCount,
       size_t borderCount, size_t tileCount);
  bool intersectRay(const Ray& ray);
  std::string toString();

  Vector3 position;
  BoundingSphere boundingSphere;
  Vector3 normal;
  Vector3 averagePosition;
  float area;
  float elevation;
  std::vector<Corner*> corners;
  std::vector<Border*> borders;
  std::vector<Tile*> tiles;
  Plate* plate;
  float temperature;
  float moisture;
  std::string biome;
  Vector3 plateMovement;
};

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TILE_H
