#include <babylon/extensions/hexplanetgeneration/terrain/heightmap.h>

#include <babylon/extensions/hexplanetgeneration/utils/fast_simplex_noise.h>

namespace BABYLON {
namespace Extensions {

Heightmap::Heightmap(const std::string& seed, float maxHeight, float height,
                     float width)
    : _seed{seed}
    , _data{{}}
    , _width{width}
    , _height{height}
    , _maxHeight{maxHeight}
{
  FastSimplexNoiseOptions noiseOptions;
  noiseOptions.amplitude   = 0.4f;
  noiseOptions.frequency   = 0.008f;
  noiseOptions.max         = maxHeight;
  noiseOptions.min         = 0.f;
  noiseOptions.octaves     = 10;
  noiseOptions.persistence = 0.5f;

  _noise = std::make_unique<FastSimplexNoise>(noiseOptions);

  generateHeightmap();
}

Heightmap::~Heightmap()
{
}

void Heightmap::generateHeightmap()
{
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

      const auto idx = static_cast<size_t>(y * _width + x);
      if (idx >= _data.size()) {
        _data.resize(idx + 1);
      }
      _data[idx] = c;
    }
  }
}

float Heightmap::getHeight(float u, float v) const
{
  const float x    = std::floor(u * _width);
  const float y    = std::floor(v * _height);
  const size_t idx = static_cast<size_t>(y * _width + x);

  return std::floor(_data[idx]);
}

float Heightmap::getHeightNormalized(float u, float v) const
{
  return getHeight(u, v) / _maxHeight;
}

} // end of namespace Extensions
} // end of namespace BABYLON
