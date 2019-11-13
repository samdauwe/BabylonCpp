#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TERRAIN_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TERRAIN_H

#include <string>
#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Color3;

namespace Extensions {

class Color;
class Heightmap;
class Rainmap;
class Temperature;

class BABYLON_SHARED_EXPORT Terrain {

public:
  Terrain(const std::string& seed, float width = 512.f, float height = 512.f);
  ~Terrain(); // = default

  /**
   * Get the color depending upon the tile from the TerrainColor object.
   */
  Color3 getColor(float u, float v) const;

  /**
   * Get height depending upon the tile, from the TerrainHeightmap object
   */
  Color3 getHeight(float u, float v) const;

private:
  std::string _seed;
  float _width;
  float _height;
  // Terrain Height Properties
  float _maxHeight;
  // Terrain Temperature Properties
  float _maxTemp;
  float _minTemp;
  float _tempDistortion;
  // Terrain Rainfall Properties
  float _maxRain;

  std::unique_ptr<Color> _terrainColor;
  std::unique_ptr<Heightmap> _terrainHeightmap;
  std::unique_ptr<Rainmap> _terrainRainmap;
  std::unique_ptr<Temperature> _terrainTemperature;

}; // end of class Terrain

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TERRAIN_H
