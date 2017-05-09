#ifndef BABYLON_EXTENSIONS_NOISE_GENERATION_FAST_NOISE_H
#define BABYLON_EXTENSIONS_NOISE_GENERATION_FAST_NOISE_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

template <typename T>
constexpr T fade(T t)
{
  return t * t * t * (t * (t * 6 - 15) + 10);
}

template <typename T>
constexpr T lerp(T t, T a, T b)
{
  return a + t * (b - a);
}

template <typename T, typename I>
constexpr T grad(I hash, T x, T y, T z)
{
  I h = hash & 15;
  T u = h < 8 ? x : y;
  T v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

class PerlinNoise {
public:
  PerlinNoise(uint32_t seed = 0);

  // 1D Perlin noise
  double noise(double x) const
  {
    return noise(x, 0, 0);
  }

  // 2D Perlin noise
  double noise(double x, double y) const
  {
    return noise(x, y, 0);
  }

  // 3D Perlin noise
  double noise(double x, double y, double z) const;

private:
  std::array<int, 512> p;
};

class PerlinNoiseOctave {
public:
  PerlinNoiseOctave(int octaves, uint32_t seed = 0);

  double noise(double x) const
  {
    return noise(x, 0, 0);
  }

  double noise(double x, double y) const
  {
    return noise(x, y, 0);
  }

  double noise(double x, double y, double z) const;

private:
  PerlinNoise _perlinNoise;
  int _octaves;
};

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NOISE_GENERATION_FAST_NOISE_H
