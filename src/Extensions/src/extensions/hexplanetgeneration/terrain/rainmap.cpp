#include <babylon/extensions/hexplanetgeneration/terrain/rainmap.h>

#include <cmath>

#include <babylon/babylon_constants.h>
#include <babylon/extensions/hexplanetgeneration/utils/fast_simplex_noise.h>

namespace BABYLON {
namespace Extensions {

Rainmap::Rainmap(const std::string& seed, float maxRain, float height,
                 float width)
    : _seed{seed}, _width{width}, _height{height}, _maxRain{maxRain}
{
  FastSimplexNoiseOptions noiseOptions;
  noiseOptions.amplitude   = 0.1f;
  noiseOptions.frequency   = 0.003f;
  noiseOptions.max         = maxRain;
  noiseOptions.min         = 0.f;
  noiseOptions.octaves     = 30;
  noiseOptions.persistence = 0.5f;

  _noise = std::make_unique<FastSimplexNoise>(noiseOptions);
}

Rainmap::~Rainmap()
{
}

void Rainmap::generateRainmap()
{
  const auto n = static_cast<size_t>(_height * _width + _width + 1);
  _data.resize(n);

  for (float x = 0.f; x < _width; ++x) {
    for (float y = 0.f; y < _height; ++y) {
      float s = x / _width;
      float t = y / _height;

      float x1 = 0.f;
      float x2 = _width;
      float y1 = 0.f;
      float y2 = _height;
      float dx = x2 - x1;
      float dy = y2 - y1;

      float nx = x1 + std::cos(s * Math::PI2) * dx / Math::PI2;
      float ny = y1 + std::cos(t * Math::PI2) * dy / Math::PI2;
      float nz = x1 + std::sin(s * Math::PI2) * dx / Math::PI2;
      float nw = y1 + std::sin(t * Math::PI2) * dy / Math::PI2;
      float c  = _noise->scaled4D(nx, ny, nz, nw);

      const size_t idx = static_cast<size_t>(y * _width + x);
      _data[idx]       = c;
    }
  }
}

float Rainmap::getRainfall(float u, float v) const
{
  const float x    = std::floor(u * _width);
  const float y    = std::floor(v * _height);
  const size_t idx = static_cast<size_t>(y * _width + x);

  return std::floor(_data[idx]);
}

float Rainmap::getRainfallNormalized(float u, float v) const
{
  return getRainfall(u, v) / _maxRain;
}

} // end of namespace Extensions
} // end of namespace BABYLON
