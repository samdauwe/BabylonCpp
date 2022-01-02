#ifndef BABYLON_EXTENSIONS_NOISE_GENERATION_PERLIN_NOISE_H
#define BABYLON_EXTENSIONS_NOISE_GENERATION_PERLIN_NOISE_H

#include <array>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

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

/**
 * @brief Perlin noise generator class.
 */
class BABYLON_SHARED_EXPORT PerlinNoise {

public:
  /**
   * @brief Default constructor. Initialize the permutation vector with the
   * reference values.
   */
  PerlinNoise();

  /**
   * @brief Constructor. Generate a new permutation vector based on the value of
   * seed.
   * @param seed Custom seed value.
   */
  PerlinNoise(uint32_t seed);

  /**
   * @brief Returns 1D Perlin noise value.
   * @param x X value.
   * @return 1D Perlin noise value.
   */
  [[nodiscard]] double noise(double x) const;

  /**
   * @brief Returns 2D Perlin noise value.
   * @param x X value.
   * @param y Y value.
   * @return 2D Perlin noise value.
   */
  [[nodiscard]] double noise(double x, double y) const;

  /**
   * @brief Returns 3D Perlin noise value.
   * @param x X value.
   * @param y Y value.
   * @param z Z value.
   * @return 3D Perlin noise value
   */
  [[nodiscard]] double noise(double x, double y, double z) const;

private:
  // The permutation vector
  std::array<int, 512> p;

}; // end of class PerlinNoise

class PerlinNoiseOctave {

public:
  PerlinNoiseOctave(int octaves, uint32_t seed = 0);

  [[nodiscard]] double noise(double x) const
  {
    return noise(x, 0, 0);
  }

  [[nodiscard]] double noise(double x, double y) const
  {
    return noise(x, y, 0);
  }

  [[nodiscard]] double noise(double x, double y, double z) const;

private:
  PerlinNoise _perlinNoise;
  int _octaves;

}; // end of class PerlinNoiseOctave

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NOISE_GENERATION_PERLIN_NOISE_H
