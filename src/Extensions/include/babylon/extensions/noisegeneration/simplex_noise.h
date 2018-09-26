#ifndef BABYLON_EXTENSIONS_NOISE_GENERATION_SIMPLEX_NOISE_H
#define BABYLON_EXTENSIONS_NOISE_GENERATION_SIMPLEX_NOISE_H

#include <array>
#include <functional>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Vector2;
class Vector3;
class Vector4;

namespace Extensions {

/**
 * @brief Simplex noise generator class.
 * Source: https://github.com/simongeilfus/SimplexNoise
 */
class BABYLON_SHARED_EXPORT SimplexNoise {

public:
  template <typename T>
  constexpr int fastfloor(T x)
  {
    return (x > 0) ? static_cast<int>(x) : static_cast<int>(x) - 1;
  }

public:
  using mat22 = std::array<float, 4>;
  using vec5  = std::array<float, 5>;

public:
  /*
   * Gradient tables. These could be programmed the Ken Perlin way with
   * some clever bit-twiddling, but this is more clear, and not really slower.
   */
  static std::array<std::array<float, 2>, 8> grad2lut;

  /*
   * Gradient directions for 3D.
   * These vectors are based on the midpoints of the 12 edges of a cube.
   * A larger array of random unit length vectors would also do the job,
   * but these 12 (including 4 repeats to make the array length a power
   * of two) work better. They are not random, they are carefully chosen
   * to represent a small, isotropic set of directions.
   */
  static std::array<std::array<float, 3>, 16> grad3lut;
  static std::array<std::array<float, 4>, 32> grad4lut;

  /*
   * For 3D, we define two orthogonal vectors in the desired rotation plane.
   * These vectors are based on the midpoints of the 12 edges of a cube,
   * they all rotate in their own plane and are never coincident or collinear.
   * A larger array of random vectors would also do the job, but these 12
   * (including 4 repeats to make the array length a power of two) work better.
   * They are not random, they are carefully chosen to represent a small
   * isotropic set of directions for any rotation angle.
   */
  static constexpr float a = 0.81649658f; // a = sqrt(2)/sqrt(3) = 0.816496580
  static std::array<std::array<float, 3>, 16> grad3u;
  static std::array<std::array<float, 3>, 32> grad3v;

  /*
   * Skewing factors for 2D simplex grid.
   */
  static constexpr float F2 = 0.366025403f; // F2 = 0.5*(sqrt(3.0)-1.0)
  static constexpr float G2 = 0.211324865f; // G2 = (3.0-Math.sqrt(3.0))/6.0

  /*
   * Skewing factors for 3D simplex grid.
   */
  static constexpr float F3 = 0.333333333f; // F3 = 1/3
  static constexpr float G3 = 0.166666667f; // G3 = 1/6

  /*
   * The skewing and unskewing factors are hairy again for the 4D case.
   */
  static constexpr float F4 = 0.309016994f; // F4 = (Math.sqrt(5.0)-1.0)/4.0
  static constexpr float G4 = 0.138196601f; // G4 = (5.0-Math.sqrt(5.0))/20.0

  static std::array<std::array<float, 4>, 64> sSimplexLut;

public:
  SimplexNoise();
  ~SimplexNoise();

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns a 1D simplex noise.
   */
  float noise(float x);

  /**
   * @brief Returns a 2D simplex noise.
   */
  float noise(const Vector2& v);

  /**
   * @brief Returns a 3D simplex noise.
   */
  float noise(const Vector3& v);

  /**
   * @brief Returns a 4D simplex noise.
   */
  float noise(const Vector4& v);

  // ---------------------------------------------------------------------------

  template <typename T>
  inline float ridgedNoise_t(const T& input)
  {
    return 1.0f - std::abs(noise(input));
  }

  /**
   * @brief Returns a 1D simplex ridged noise.
   */
  float ridgedNoise(float x);

  /**
   * @brief Returns a 2D simplex ridged noise.
   */
  float ridgedNoise(const Vector2& v);

  /**
   * @brief Returns a 3D simplex ridged noise.
   */
  float ridgedNoise(const Vector3& v);

  /**
   * @brief Returns a 4D simplex ridged noise.
   */
  float ridgedNoise(const Vector4& v);

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns a 1D simplex noise with analytical derivative.
   */
  Vector2 dnoise(float x);

  /**
   * @brief Returns a 2D simplex noise with analytical derivatives.
   */
  Vector3 dnoise(const Vector2& v);

  /**
   * @brief Returns a 3D simplex noise with analytical derivatives.
   */
  Vector4 dnoise(const Vector3& v);

  /**
   * @brief Returns a 4D simplex noise with analytical derivatives.
   */
  vec5 dnoise(const Vector4& v);

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns a 2D simplex noise with rotating gradients.
   */
  float flowNoise(const Vector2& v, float angle);

  /**
   * @brief Returns a 3D simplex noise with rotating gradients.
   */
  float flowNoise(const Vector3& v, float angle);

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns a 2D simplex noise with rotating gradients and analytical
   * derivatives.
   */
  Vector3 dFlowNoise(const Vector2& v, float angle);

  /**
   * @brief Returns a 3D simplex noise with rotating gradients and analytical
   * derivatives.
   */
  Vector4 dFlowNoise(const Vector3& v, float angle);

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns the curl of a 2D simplex noise.
   */
  Vector2 curlNoise(const Vector2& v);

  /**
   * @brief Returns the curl of a 2D simplex flow noise.
   */
  Vector2 curlNoise(const Vector2& v, float t);

  /**
   * @brief Returns the curl of a 2D simplex noise fractal brownian motion sum.
   */
  Vector2 curlNoise(const Vector2& v, uint8_t octaves, float lacunarity,
                    float gain);

  /**
   * @brief Returns the curl of a 3D simplex noise.
   */
  Vector3 curlNoise(const Vector3& v);

  /**
   * @brief Returns the curl of a 3D simplex flow noise.
   */
  Vector3 curlNoise(const Vector3& v, float t);

  /**
   * @brief Returns the curl approximation of a 3D simplex noise fractal
   * brownian motion sum.
   */
  Vector3 curlNoise(const Vector3& v, uint8_t octaves, float lacunarity,
                    float gain);

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns the curl of a custom 2D potential using finite difference
   * approximation.
   */
  Vector2 curl(const Vector2& v,
               const std::function<float(const Vector2&)>& potential,
               float delta = 1e-4f);

  /**
   * @brief Returns the curl of a custom 3D potential using finite difference
   * approximation.
   */
  Vector3 curl(const Vector3& v,
               const std::function<Vector3(const Vector3&)>& potential,
               float delta = 1e-4f);

  // ---------------------------------------------------------------------------

  template <typename T>
  float fBm_t(const T& input, uint8_t octaves, float lacunarity, float gain)
  {
    float sum  = 0.0f;
    float freq = 1.0f;
    float amp  = 0.5f;

    for (uint8_t i = 0; i < octaves; i++) {
      float n = noise(input * freq);
      sum += n * amp;
      freq *= lacunarity;
      amp *= gain;
    }

    return sum;
  }

  /**
   * @brief Returns a 1D simplex noise fractal brownian motion sum.
   */
  float fBm(float x, uint8_t octaves = 4, float lacunarity = 2.0f,
            float gain = 0.5f);

  /**
   * @brief Returns a 2D simplex noise fractal brownian motion sum.
   */
  float fBm(const Vector2& v, uint8_t octaves = 4, float lacunarity = 2.0f,
            float gain = 0.5f);

  /**
   * @brief Returns a 3D simplex noise fractal brownian motion sum.
   */
  float fBm(const Vector3& v, uint8_t octaves = 4, float lacunarity = 2.0f,
            float gain = 0.5f);

  /**
   * @brief Returns a 4D simplex noise fractal brownian motion sum.
   */
  float fBm(const Vector4& v, uint8_t octaves = 4, float lacunarity = 2.0f,
            float gain = 0.5f);

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns a 1D simplex noise fractal brownian motion sum with
   * analytical derivatives.
   */
  Vector2 dfBm(float x, uint8_t octaves = 4, float lacunarity = 2.0f,
               float gain = 0.5f);

  /**
   * @brief Returns a 2D simplex noise fractal brownian motion sum with
   * analytical derivatives.
   */
  Vector3 dfBm(const Vector2& v, uint8_t octaves = 4, float lacunarity = 2.0f,
               float gain = 0.5f);

  /**
   * @brief Returns a 3D simplex noise fractal brownian motion sum with
   * analytical derivatives.
   */
  Vector4 dfBm(const Vector3& v, uint8_t octaves = 4, float lacunarity = 2.0f,
               float gain = 0.5f);

  /**
   * @brief Returns a 4D simplex noise fractal brownian motion sum with
   * analytical derivatives.
   */
  vec5 dfBm(const Vector4& v, uint8_t octaves = 4, float lacunarity = 2.0f,
            float gain = 0.5f);

  // ---------------------------------------------------------------------------

  float ridge(float h, float offset)
  {
    h = offset - std::abs(h);
    return h * h;
  }

  template <typename T>
  float ridgedMF_t(const T& input, float ridgeOffset, uint8_t octaves,
                   float lacunarity, float gain)
  {
    float sum  = 0.f;
    float freq = 1.f;
    float amp  = 0.5f;
    float prev = 1.f;

    for (uint8_t i = 0; i < octaves; i++) {
      float n = ridge(noise(input * freq), ridgeOffset);
      sum += n * amp * prev;
      prev = n;
      freq *= lacunarity;
      amp *= gain;
    }
    return sum;
  }

  /**
   * @brief Returns a 1D simplex ridged multi-fractal noise sum.
   */
  float ridgedMF(float x, float ridgeOffset = 1.0f, uint8_t octaves = 4,
                 float lacunarity = 2.0f, float gain = 0.5f);

  /**
   * @brief Returns a 2D simplex ridged multi-fractal noise sum.
   */
  float ridgedMF(const Vector2& v, float ridgeOffset = 1.0f,
                 uint8_t octaves = 4, float lacunarity = 2.0f,
                 float gain = 0.5f);

  /**
   * @brief Returns a 3D simplex ridged multi-fractal noise sum.
   */
  float ridgedMF(const Vector3& v, float ridgeOffset = 1.0f,
                 uint8_t octaves = 4, float lacunarity = 2.0f,
                 float gain = 0.5f);

  /**
   * @brief Returns a 4D simplex ridged multi-fractal noise sum.
   */
  float ridgedMF(const Vector4& v, float ridgeOffset = 1.0f,
                 uint8_t octaves = 4, float lacunarity = 2.0f,
                 float gain = 0.5f);

  // ---------------------------------------------------------------------------

  /**
   * @brief Returns the 2D simplex noise fractal brownian motion sum variation
   * by Iñigo Quilez.
   */
  float iqfBm(const Vector2& v, uint8_t octaves = 4, float lacunarity = 2.0f,
              float gain = 0.5f);

  /**
   * @brief Returns the 2D simplex noise fractal brownian motion sum variation
   * by Iñigo Quilez.
   */
  float iqfBm(const Vector3& v, uint8_t octaves = 4, float lacunarity = 2.0f,
              float gain = 0.5f);

  // ---------------------------------------------------------------------------

  /**
   * @brief Seeds the permutation table with new random values.
   */
  void seed(uint32_t s);

private:
  /*
   * Helper functions to compute gradients-dot-residualvectors (1D to 4D)
   * Note that these generate gradients of more than unit length. To make
   * a close match with the value range of classic Perlin noise, the final
   * noise values need to be rescaled to fit nicely within [-1,1].
   * (The simplex noise functions as such also have different scaling.)
   * Note also that these noise functions are the most practical and useful
   * signed version of Perlin noise. To return values according to the
   * RenderMan specification from the SL noise() and pnoise() functions,
   * the noise values need to be scaled and offset to [0,1], like this:
   * float SLnoise = (SimplexNoise1234::noise(x,y,z) + 1.0) * 0.5;
   */
  float grad(int hash, float x) const;
  float grad(int hash, float x, float y) const;
  float grad(int hash, float x, float y, float z) const;
  float grad(int hash, float x, float y, float z, float t) const;

  /*
   * Helper functions to compute gradients in 1D to 4D
   * and gradients-dot-residualvectors in 2D to 4D.
   */
  void grad1(int hash, float* gx);
  void grad2(int hash, float* gx, float* gy);
  void grad3(int hash, float* gx, float* gy, float* gz);
  void grad4(int hash, float* gx, float* gy, float* gz, float* gw);

  /*
   * Helper functions to compute rotated gradients and
   * gradients-dot-residualvectors in 2D and 3D.
   */
  void gradrot2(int hash, float sin_t, float cos_t, float* gx, float* gy);
  void gradrot3(int hash, float sin_t, float cos_t, float* gx, float* gy,
                float* gz);
  float graddotp2(float gx, float gy, float x, float y) const;
  float graddotp3(float gx, float gy, float gz, float x, float y,
                  float z) const;

private:
  /*
   * Permutation table. This is just a random jumble of all numbers 0-255,
   * repeated twice to avoid wrapping the index at 255 for each lookup.
   * This needs to be exactly the same for all instances on all platforms,
   * so it's easiest to just keep it as static explicit data.
   * This also removes the need for any initialisation of this class.
   *
   * Note that making this an int[] instead of a char[] might make the
   * code run faster on platforms with a high penalty for unaligned single
   * byte addressing. Intel x86 is generally single-byte-friendly, but
   * some other CPUs are faster with 4-aligned reads.
   * However, a char[] is smaller, which avoids cache trashing, and that
   * is probably the most important aspect on most architectures.
   * This array is accessed a *lot* by the noise functions.
   * A vector-valued noise over 3D accesses it 96 times, and a
   * float-valued 4D noise 64 times. We want this to fit in the cache!
   */
  std::array<unsigned char, 512> perm;

}; // end of class SimplexNoise

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NOISE_GENERATION_SIMPLEX_NOISE_H
