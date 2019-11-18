#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_SIMPLE_PLANET_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_SIMPLE_PLANET_H

// https://experilous.com/1/blog/post/procedural-planet-generation

#include <babylon/babylon_api.h>
#include <babylon/extensions/hexplanetgeneration/icosphere.h>
#include <babylon/extensions/hexplanetgeneration/utils/xor_shift_128.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class LinesMesh;
class Mesh;
class Scene;
using LinesMeshPtr = std::shared_ptr<LinesMesh>;
using MeshPtr      = std::shared_ptr<Mesh>;

namespace Extensions {

struct Icosphere;
class Terrain;

struct PlanetCorner {
  Vector3 position;
  Vector2 uv;
}; // end of struct PlanetCorner

struct PlanetTile {
  size_t id;
  Vector3 center;
  std::vector<PlanetCorner> corners;
}; // end of struct PlanetTile

struct PlanetData {
  Uint32Array faceToTile;
  MeshPtr mesh                = nullptr;
  LinesMeshPtr selectedBorder = nullptr;
  std::vector<PlanetTile> tiles;
};

class BABYLON_SHARED_EXPORT SimplePlanet {

public:
  SimplePlanet(float scale, float degree, Scene* scene, const std::string& seed);
  ~SimplePlanet(); // = default

  void initialize();
  [[nodiscard]] Vector2 calculateUVCoord(const Vector3& p) const;
  void revolve();
  void pickTile(unsigned int faceId);
  void render();

public:
  float _degree;

private:
  float _scale;
  Scene* _scene;
  std::string _seed;
  XorShift128 _random;
  IcosahedronMesh _icosahedronMesh;
  PlanetData _planet;
  std::unique_ptr<Terrain> _terrain;

}; // end of class SimplePlanet

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_SIMPLE_PLANET_H
