#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_SIMPLEX_NOISE_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_SIMPLEX_NOISE_H

/*
 * A speed-improved simplex noise algorithm for 2D, 3D and 4D.
 *
 * Based on example code by Stefan Gustavson (stegu@itn.liu.se).
 * Optimisations by Peter Eastman (peastman@drizzle.stanford.edu).
 * Better rank ordering method by Stefan Gustavson in 2012.
 *
 * This code was placed in the public domain by its original author,
 * Stefan Gustavson. You may use it as you see fit, but
 * attribution is appreciated.
 */

#include <array>
#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {
namespace Extensions {

struct FastSimplexNoiseOptions {
  FastSimplexNoiseOptions();
  ~FastSimplexNoiseOptions();

  // The base amplitude (default: 1.0)
  float amplitude;
  // The base frequency (default: 1.0)
  float frequency;
  // The maximum scaled value to return (effective default: 1.0)
  float max;
  // The minimum scaled value to return (effective default: -1.0)
  float min;
  // The number of octaves to sum for noise generation (default: 1)
  unsigned int octaves;
  // The persistence of amplitude per octave (default: 0.5)
  float persistence;
  // A function that generates random values between 0 and 1 (default:
  // std random)
  std::function<float()> random;
}; // end of struct FastSimplexNoise

/**
 * @brief C++ implementation of the improved, faster Simplex algorithm outlined
 * in Stefan Gustavson's Simplex noise demystified.
 *
 * Source:
 * https://github.com/joshforisha/fast-simplex-noise-js
 */
class FastSimplexNoise {

public:
  static const float G2;
  static const float G3;
  static const float G4;
  static const std::array<std::array<float, 3>, 12> GRAD3;
  static const std::array<std::array<float, 4>, 32> GRAD4;

public:
  // dot2D calculation
  static constexpr float dot2D(const std::array<float, 3>& g, float x, float y)
  {
    return g[0] * x + g[1] * y;
  }

  // dot2D calculation
  static constexpr float dot3D(const std::array<float, 3>& g, float x, float y,
                               float z)
  {
    return g[0] * x + g[1] * y + g[2] * z;
  }

  // dot2D calculation
  static constexpr float dot4D(const std::array<float, 4>& g, float x, float y,
                               float z, float w)
  {
    return g[0] * x + g[1] * y + g[2] * z + g[3] * w;
  }

public:
  FastSimplexNoise(const FastSimplexNoiseOptions& options);
  ~FastSimplexNoise();

  /**
   * Get a scaled noise value for a 2D or 3D point at coords on the surface of a
   * cylinder with circumference.
   */
  float cylindrical(float c, const Float32Array& coords) const;

  /**
   * Get a noise value between min and max for a point (x,y) on the surface of a
   * cylinder with circumference c.
   */
  float cylindrical2D(float c, float x, float y) const;

  /**
   * Get a noise value between min and max for a point (x, y, z) on the surface
   * of a cylinder with circumference c.
   */
  float cylindrical3D(float c, float x, float y, float z) const;

  /**
   * Get a noise value [-1, 1] at a 2D, 3D, or 4D point at coords.
   */
  float raw(const Float32Array& coords) const;

  /**
   * Get a noise value [-1, 1] at the 2D coordinate (x,y).
   */
  float raw2D(float x, float y) const;

  /**
   * Get a noise value [-1, 1] at the 3D coordinate (x,y,z).
   */
  float raw3D(float x, float y, float z) const;

  /**
   * Get a noise value [-1, 1] at the 4D coordinate (x,y,z,w).
   */
  float raw4D(float x, float y, float z, float w) const;

  /**
   * Get a scaled noise value (using options) at a 2D, 3D, or 4D point at
   * coords.
   */
  float scaled(const Float32Array& coords) const;

  /**
   * Specific scaled() call for a 2D point at (x, y).
   */
  float scaled2D(float x, float y) const;

  /**
   * Specific scaled() call for a 3D point at (x, y, z).
   */
  float scaled3D(float x, float y, float z) const;

  /**
   * Specific scaled() call for a 4D point at (x, y, z, w).
   */
  float scaled4D(float x, float y, float z, float w) const;

  /**
   * Get a scaled noise value (using options) at a 2D or 3D point at coords on
   * the surface of a sphere with circumference.
   */
  float spherical(float c, const Float32Array& coords) const;

  /**
   * Specific spherical() call for a 2D point at (x, y).
   */
  float spherical2D(float c, float x, float y) const;

  /**
   * Specific spherical() call for a 3D point at (x, y, z).
   */
  float spherical3D(float c, float x, float y, float z) const;

private:
  // The base amplitude
  float _amplitude;
  // The base frequency
  float _frequency;
  // The maximum scaled value to return
  float _max;
  // The minimum scaled value to return
  float _min;
  // The number of octaves to sum for noise generation
  unsigned int _octaves;
  // The persistence of amplitude per octave
  float _persistence;
  // A function that generates random values between 0 and 1
  std::function<float()> _random;
  // Other members
  std::function<float(float value)> _scale;
  Uint8Array _perm;
  Uint8Array _permMod12;

}; // end of class FastSimplexNoise

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_UTILS_SIMPLEX_NOISE_H
