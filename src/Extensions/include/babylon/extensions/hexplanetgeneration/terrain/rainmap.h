#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_RAINMAP_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_RAINMAP_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {
namespace Extensions {

class FastSimplexNoise;

class Rainmap {

public:
  Rainmap(const std::string& seed, float maxRain, float height, float width);
  ~Rainmap(); // = default

  void generateRainmap();
  [[nodiscard]] float getRainfall(float u, float v) const;
  [[nodiscard]] float getRainfallNormalized(float u, float v) const;

private:
  std::unique_ptr<FastSimplexNoise> _noise;
  std::string _seed;
  Float32Array _data;
  float _width;
  float _height;
  float _maxRain;

}; // end of class Rainmap

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_RAINMAP_H
