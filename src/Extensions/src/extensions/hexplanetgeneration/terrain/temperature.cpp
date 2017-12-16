#include <babylon/extensions/hexplanetgeneration/terrain/temperature.h>

#include <babylon/extensions/hexplanetgeneration/utils/fast_simplex_noise.h>
#include <babylon/extensions/hexplanetgeneration/utils/gradient.h>

namespace BABYLON {
namespace Extensions {

Temperature::Temperature(const std::string& seed, float maxTemp, float minTemp,
                         float distortion, float height, float width)
    : _seed{seed}
    , _data{{}}
    , _minTemp{minTemp}
    , _maxTemp{maxTemp}
    , _width{width}
    , _height{height}
{
  FastSimplexNoiseOptions noiseOptions;
  noiseOptions.amplitude   = 0.1f;
  noiseOptions.frequency   = 0.004f;
  noiseOptions.max         = distortion;
  noiseOptions.min         = -distortion;
  noiseOptions.octaves     = 10;
  noiseOptions.persistence = 0.5f;

  _noise = std::make_unique<FastSimplexNoise>(noiseOptions);

  setupTemperatureGradient();
  generateTemperature();
}

Temperature::~Temperature()
{
}

void Temperature::setupTemperatureGradient()
{
  _gradient = std::make_unique<Gradient<float>>(_height);

  _gradient->addStop(0.f, _minTemp);
  _gradient->addStop(0.2f, _minTemp);
  _gradient->addStop(0.5f, _maxTemp);
  _gradient->addStop(0.8f, _minTemp);
  _gradient->addStop(1.f, _minTemp);

  _gradient->calculate();
}

void Temperature::generateTemperature()
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

      const size_t idx = static_cast<size_t>(y * _width + x);
      _data[idx]       = c;
    }
  }
}

float Temperature::getTemperature(float u, float v) const
{
  const float x    = std::floor(u * _width);
  const float y    = std::floor(v * _height);
  const size_t idx = static_cast<size_t>(y * _width + x);

  return _data[idx] + _gradient->getValue(static_cast<size_t>(v));
}

float Temperature::getTemperatureNormalized(float u, float v) const
{
  float data = (getTemperature(u, v) - _minTemp) / (_maxTemp - _minTemp);
  if (data < 0.f) {
    data = 0.f;
  }
  else if (data > 1.f) {
    data = 1.f;
  }
  return data;
}

} // end of namespace Extensions
} // end of namespace BABYLON
