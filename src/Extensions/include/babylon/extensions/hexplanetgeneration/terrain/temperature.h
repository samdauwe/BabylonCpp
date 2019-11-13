#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TEMPERATURE_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TEMPERATURE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/extensions/hexplanetgeneration/utils/gradient.h>

namespace BABYLON {
namespace Extensions {

class FastSimplexNoise;

class Temperature {

public:
  Temperature(const std::string& seed, float minTemp, float maxTemp,
              float distortion, float height, float width);
  ~Temperature(); // = default

  void setupTemperatureGradient();
  void generateTemperature();
  float getTemperature(float u, float v) const;
  float getTemperatureNormalized(float u, float v) const;

private:
  std::unique_ptr<FastSimplexNoise> _noise;
  std::unique_ptr<Gradient<float>> _gradient;
  std::string _seed;
  Float32Array _data;
  float _minTemp;
  float _maxTemp;
  float _width;
  float _height;

}; // end of class Temperature

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_TERRAIN_TEMPERATURE_H
