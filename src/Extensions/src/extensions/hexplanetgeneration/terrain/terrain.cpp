#include <babylon/extensions/hexplanetgeneration/terrain/terrain.h>

#include <babylon/extensions/hexplanetgeneration/terrain/color.h>
#include <babylon/extensions/hexplanetgeneration/terrain/heightmap.h>
#include <babylon/extensions/hexplanetgeneration/terrain/rainmap.h>
#include <babylon/extensions/hexplanetgeneration/terrain/temperature.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace Extensions {

Terrain::Terrain(const std::string& seed, float width, float height)
    : _seed{seed}
    , _width{width}
    , _height{height}
    , _maxHeight{255.f}
    , _maxTemp{40.f}
    , _minTemp{-10.f}
    , _tempDistortion{40.f}
    , _maxRain{400}
{
  _terrainColor = std::make_unique<Color>(_maxHeight);
  _terrainColor->setupGradient();
  _terrainHeightmap
    = std::make_unique<Heightmap>(_seed, _maxHeight, _height, _width);
  _terrainHeightmap->generateHeightmap();
  _terrainRainmap
    = std::make_unique<Rainmap>(_seed + "rain", _maxRain, _height, _width);
  _terrainRainmap->generateRainmap();
  _terrainTemperature = std::make_unique<Temperature>(
    _seed + "temp", _maxTemp, _minTemp, _tempDistortion, _height, _width);
  _terrainTemperature->setupTemperatureGradient();
  _terrainTemperature->generateTemperature();
}

Terrain::~Terrain()
{
}

Color3 Terrain::getColor(float u, float v) const
{
  const float height = _terrainHeightmap->getHeightNormalized(u, v);
  const float rain   = _terrainRainmap->getRainfallNormalized(u, v);
  const float temp   = _terrainTemperature->getTemperatureNormalized(u, v);

  return _terrainColor->getColor(height, rain, temp);
}

Color3 Terrain::getHeight(float u, float v) const
{
  return _terrainHeightmap->getHeightNormalized(u, v);
}

} // end of namespace Extensions
} // end of namespace BABYLON
