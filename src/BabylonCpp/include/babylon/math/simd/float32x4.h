#ifndef BABYLON_MATH_SIMD_FLOAT32X4_H
#define BABYLON_MATH_SIMD_FLOAT32X4_H

#include <babylon/babylon_global.h>

#include <xmmintrin.h>

#define ALIGN_16 __attribute__((aligned(16)))

namespace BABYLON {
namespace SIMD {
namespace Float32x4 {

typedef __m128 float32x4_t;

static inline float32x4_t load(const std::array<float, 4>& f)
{
  return _mm_load_ps(f.data());
}

static inline float32x4_t load(const std::array<float, 16>& m, unsigned int i)
{
  float ALIGN_16 f[4] = {m[i], m[i + 1], m[i + 2], m[i + 3]};
  return _mm_load_ps(f);
}

static inline void store(std::array<float, 4>& dest, float32x4_t src)
{
  _mm_store_ps(dest.data(), src);
}

static inline float32x4_t sub(float32x4_t a, float32x4_t b)
{
  return _mm_sub_ps(a, b);
}

static inline float32x4_t mul(float32x4_t a, float32x4_t b)
{
  return _mm_mul_ps(a, b);
}

static inline float32x4_t reciprocalSqrtApproximation(float32x4_t a)
{
  return _mm_rsqrt_ps(a);
}

} // end of namespace Float32x4
} // end of namespace SIMD
} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SIMD_FLOAT32X4_H
