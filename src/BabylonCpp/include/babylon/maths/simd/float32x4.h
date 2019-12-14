#ifndef BABYLON_MATH_SIMD_FLOAT32X4_H
#define BABYLON_MATH_SIMD_FLOAT32X4_H

#ifdef OPTION_ENABLE_SIMD

#include <babylon/babylon_api.h>

#include <xmmintrin.h>

#define ALIGN_16 __attribute__((aligned(16)))

namespace BABYLON {
namespace SIMD {

/**
 * @brief Represents a 128-bit vector divided into 4 lanes storing single
 * precision floating point values.
 *
 * compile with flags: g++ -msse3 -mfpmath=sse
 */
struct BABYLON_SHARED_EXPORT Float32x4 {

  typedef __m128 float32x4_t;

  float32x4_t xmm;

  /** Shuffle types **/
  enum class ShuffleType {
    LANE_0145,
    LANE_0246,
    LANE_1357,
    LANE_2367,
  }; // end of enum class ShuffleType

  /**
   * @brief Constructor
   */
  Float32x4()
  {
    xmm = _mm_setzero_ps();
  }

  Float32x4(float32x4_t v) : xmm(v)
  {
  }

  Float32x4(const Float32x4& v) : xmm{v.xmm}
  {
  }

  Float32x4(float v)
  {
    xmm = _mm_set1_ps(v);
  }

  /**
   * @brief Constructor
   * @param x A float specifying the value of the first lane.
   * @param y A float specifying the value of the second lane.
   * @param z A float specifying the value of the third lane.
   * @param w A float specifying the value of the fourth lane.
   */
  Float32x4(float x, float y, float z, float w)
  {
    xmm = _mm_set_ps(w, z, y, x);
  }

  Float32x4(const float* v)
  {
    xmm = _mm_load_ps(v);
  }

  Float32x4 hadd(const Float32x4& v) const
  {
    return Float32x4(_mm_hadd_ps(xmm, v.xmm));
  }

  Float32x4 hsub(const Float32x4& v) const
  {
    return Float32x4(_mm_hsub_ps(xmm, v.xmm));
  }

  float sum() const
  {
    float c;
    Float32x4 s = hadd(*this).hadd(*this);
    _mm_store_ss(&c, s.xmm);
    return c;
  }

  float dot(const Float32x4& v) const
  {
    return (*this * v).sum();
  }

  Float32x4 recip() const
  {
    return Float32x4(_mm_rcp_ps(xmm));
  }

  Float32x4 sqrt() const
  {
    return Float32x4(_mm_sqrt_ps(xmm));
  }

  Float32x4 rsqrt() const
  {
    return Float32x4(_mm_rsqrt_ps(xmm));
  }

  Float32x4 min(const Float32x4& v) const
  {
    return Float32x4(_mm_min_ps(xmm, v.xmm));
  }

  Float32x4 max(const Float32x4& v) const
  {
    return Float32x4(_mm_max_ps(xmm, v.xmm));
  }

  Float32x4 andnot(const Float32x4& v) const
  {
    return Float32x4(_mm_andnot_ps(xmm, v.xmm));
  }

  Float32x4 operator&(const Float32x4& v) const
  {
    return Float32x4(_mm_and_ps(xmm, v.xmm));
  }

  Float32x4 operator|(const Float32x4& v) const
  {
    return Float32x4(_mm_or_ps(xmm, v.xmm));
  }

  Float32x4 operator^(const Float32x4& v) const
  {
    return Float32x4(_mm_xor_ps(xmm, v.xmm));
  }

  Float32x4 operator*(const Float32x4& v) const
  {
    return Float32x4(_mm_mul_ps(xmm, v.xmm));
  }

  Float32x4 operator+(const Float32x4& v) const
  {
    return Float32x4(_mm_add_ps(xmm, v.xmm));
  }

  Float32x4 operator-(const Float32x4& v) const
  {
    return Float32x4(_mm_sub_ps(xmm, v.xmm));
  }

  Float32x4 operator/(const Float32x4& v) const
  {
    return Float32x4(_mm_div_ps(xmm, v.xmm));
  }

  void operator*=(const Float32x4& v)
  {
    xmm = _mm_mul_ps(xmm, v.xmm);
  }

  void operator+=(const Float32x4& v)
  {
    xmm = _mm_add_ps(xmm, v.xmm);
  }

  void operator-=(const Float32x4& v)
  {
    xmm = _mm_sub_ps(xmm, v.xmm);
  }

  void operator/=(const Float32x4& v)
  {
    xmm = _mm_div_ps(xmm, v.xmm);
  }

  float x() const
  {
    return (reinterpret_cast<const float*>(&xmm))[0];
  }
  float y() const
  {
    return (reinterpret_cast<const float*>(&xmm))[1];
  }
  float z() const
  {
    return (reinterpret_cast<const float*>(&xmm))[2];
  }
  float w() const
  {
    return (reinterpret_cast<const float*>(&xmm))[3];
  }

  /** Statics  **/

  /**
   * @brief Create a new SIMD data type with the lane values loaded from an
   * array.
   * @param f An instance of an array.
   * @return A new SIMD data type.
   */
  static inline float32x4_t load(const std::array<float, 4>& f)
  {
    const float ALIGN_16 data[4] = {f[0], f[1], f[2], f[3]};
    return _mm_load_ps(data);
  }

  /**
   * @brief Create a new SIMD data type with the lane values loaded from an
   * array.
   * @param m An instance of an array.
   * @param i A number for the index from where to start loading in the array.
   * @return A new SIMD data type.
   */
  static inline float32x4_t load(const std::array<float, 16>& m, unsigned int i)
  {
    const float ALIGN_16 f[4] = {m[i], m[i + 1], m[i + 2], m[i + 3]};
    return _mm_load_ps(f);
  }

  /**
   * @brief Store a SIMD data type into an array.
   * @param dest An instance of an array.
   * @param src An instance of a SIMD data type to store into the array.
   */
  static inline void store(std::array<float, 4>& dest, float32x4_t src)
  {
    _mm_store_ps(dest.data(), src);
  }

  /**
   * @brief Store a SIMD data type into an array.
   * @param dest An instance of an array.
   * @param i A number for the index from where to start storing in the array.
   * @param src An instance of a SIMD data type to store into the array.
   */
  static inline void store(std::array<float, 16>& dest, unsigned int i,
                           float32x4_t src)
  {
    float p[4] = {0.f, 0.f, 0.f, 0.f};
    _mm_store_ps(p, src);
    dest[i]     = p[0];
    dest[i + 1] = p[1];
    dest[i + 2] = p[2];
    dest[i + 3] = p[3];
  }

  /**
   * @brief Returns a new SIMD data type with the given lane value replaced.
   * @param src An instance of a corresponding SIMD type.
   * @param i An index number for which lane to replace.
   * @param v A new value to be used for the lane.
   * @return A new SIMD data type with the given lane value replaced.
   */
  static inline float32x4_t replaceLane(float32x4_t src, unsigned int i,
                                        float v)
  {
    float ALIGN_16 dest[4] = {0.f, 0.f, 0.f, 0.f};
    _mm_store_ps(dest, src);
    dest[i] = v;
    return _mm_load_ps(dest);
  }

  /**
   * @brief Returns the value of the given lane.
   * @param src An instance of a corresponding SIMD type.
   * @param i An index number for which lane to extract. Lanes are numbered
   * starting at 0.
   * @return The value of the extracted lane.
   */
  static inline float extractLane(float32x4_t src, unsigned int i)
  {
    return (reinterpret_cast<const float*>(&src))[i];
  }

  /**
   * @brief Returns a new instance with the lane values negated.
   * @param a An instance of a corresponding SIMD type.
   * @return A new corresponding SIMD data type with the lane values negated.
   */
  static inline float32x4_t neg(float32x4_t a)
  {

    return _mm_mul_ps(a, _mm_set1_ps(-1.f));
  }

  /**
   * @brief Returns a new instance with the lane values negated.
   * @param a An instance of a corresponding SIMD type.
   * @return A new corresponding SIMD data type with the lane values negated.
   */
  static inline float32x4_t neg(Float32x4 a)
  {
    return _mm_mul_ps(a.xmm, _mm_set1_ps(-1.f));
  }

  /**
   * @brief Returns a new instance with the lane values added (a + b).
   * @param a An instance of a SIMD type.
   * @param b Another instance of a SIMD type.
   * @return A new corresponding SIMD data type with the lane values of a and b
   * added (a + b).
   */
  static inline float32x4_t add(float32x4_t a, float32x4_t b)
  {
    return _mm_add_ps(a, b);
  }

  /**
   * @brief Returns a new instance with the lane values subtracted (a - b).
   * @param a An instance of a SIMD type.
   * @param b Another instance of a SIMD type.
   * @return A new corresponding SIMD data type with the lane values of a and b
   * subtracted (a - b).
   */
  static inline float32x4_t sub(float32x4_t a, float32x4_t b)
  {
    return _mm_sub_ps(a, b);
  }

  /**
   * @brief Returns a new instance with the lane values subtracted (a - b).
   * @param a An instance of a SIMD type.
   * @param b Another instance of a SIMD type.
   * @return A new corresponding SIMD data type with the lane values of a and b
   * subtracted (a - b).
   */
  static inline float32x4_t sub(Float32x4 a, Float32x4 b)
  {
    return _mm_sub_ps(a.xmm, b.xmm);
  }

  /**
   * @brief Returns a new instance with the lane values multiplied (a * b).
   * @param a An instance of a SIMD type.
   * @param b Another instance of a SIMD type.
   * @return A new corresponding SIMD data type with the lane values of a and b
   * multiplied (a * b).
   */
  static inline float32x4_t mul(float32x4_t a, float32x4_t b)
  {
    return _mm_mul_ps(a, b);
  }

  /**
   * @brief Returns a new instance with the lane values multiplied (a * b).
   * @param a An instance of a SIMD type.
   * @param b Another instance of a SIMD type.
   * @return A new corresponding SIMD data type with the lane values of a and b
   * multiplied (a * b).
   */
  static inline float32x4_t mul(Float32x4 a, Float32x4 b)
  {
    return _mm_mul_ps(a.xmm, b.xmm);
  }

  /**
   * @brief Returns a new instance with the lane values divided (a / b). This
   * function is defined only on floating point SIMD types.
   * @param a An instance of a SIMD type.
   * @param b Another instance of a SIMD type.
   * @return A new corresponding SIMD data type with the lane values of a and b
   * divided (a / b).
   */
  static inline float32x4_t div(float32x4_t a, float32x4_t b)
  {
    return _mm_div_ps(a, b);
  }

  /**
   * @brief Creates an float32x4_t with all lanes set to a given value.
   * @param v An instance of a SIMD type.
   * @return
   */
  static inline float32x4_t splat(float v)
  {
    return _mm_set1_ps(v);
  }

  /**
   * @brief Creates a new SIMD data type instance with the lane values shuffled.
   * @param a An instance of a SIMD type to be shuffled.
   * @param b Another instance of a SIMD type to be shuffled.
   * @param s The shuffle type.
   * @return A new SIMD data type with the lane values shuffled.
   */
  static inline float32x4_t shuffle(float32x4_t a, float32x4_t b, ShuffleType s)
  {
    switch (s) {
      case ShuffleType::LANE_0145:
        return _mm_shuffle_ps(a, b, _MM_SHUFFLE(5, 4, 1, 0));
      case ShuffleType::LANE_0246:
        return _mm_shuffle_ps(a, b, _MM_SHUFFLE(6, 4, 2, 0));
      case ShuffleType::LANE_1357:
        return _mm_shuffle_ps(a, b, _MM_SHUFFLE(7, 5, 3, 1));
      case ShuffleType::LANE_2367:
        return _mm_shuffle_ps(a, b, _MM_SHUFFLE(7, 6, 3, 2));
    }

    return _mm_setzero_ps();
  }

  /**
   * @brief Creates a new SIMD data type instance with the lane values swizzled
   * (re-ordered).
   * @param a An instance of a SIMD type to be shuffled.
   * @param b Another instance of a SIMD type to be swizzled / re-ordered.
   * @param x Concatenated index of a and b for lane s0.
   * @param y Concatenated index of a and b for lane s1.
   * @param z Concatenated index of a and b for lane s2.
   * @param w Concatenated index of a and b for lane s3.
   * @return A new SIMD data type with the lane values values swizzled
   * (re-ordered).
   */
  static inline float32x4_t swizzle(Float32x4 a, unsigned int x, unsigned int y,
                                    unsigned int z, unsigned int w)
  {
    float ALIGN_16 p[4] = {0.f, 0.f, 0.f, 0.f};
    _mm_store_ps(p, a.xmm);
    const float ALIGN_16 res[4] = {p[x], p[y], p[z], p[w]};

    return _mm_load_ps(res);
  }

  /**
   * @brief Creates a new SIMD data type instance with the lane values swizzled
   * (re-ordered).
   * @param a An instance of a SIMD type to be shuffled.
   * @param b Another instance of a SIMD type to be swizzled / re-ordered.
   * @param x Concatenated index of a and b for lane s0.
   * @param y Concatenated index of a and b for lane s1.
   * @param z Concatenated index of a and b for lane s2.
   * @param w Concatenated index of a and b for lane s3.
   * @return A new SIMD data type with the lane values values swizzled
   * (re-ordered).
   */
  static inline float32x4_t swizzle(float32x4_t a, unsigned int x,
                                    unsigned int y, unsigned int z,
                                    unsigned int w)
  {
    float ALIGN_16 p[4] = {0.f, 0.f, 0.f, 0.f};
    _mm_store_ps(p, a);
    const float ALIGN_16 res[4] = {p[x], p[y], p[z], p[w]};

    return _mm_load_ps(res);
  }

  /**
   * @brief Returns a new instance with an approximation of the reciprocal lane
   * values (1 / x).
   * @param a An instance of a corresponding SIMD type.
   * @return A new corresponding SIMD data type with reciprocal lane values (1 /
   * x).
   */
  static inline float32x4_t reciprocalApproximation(float32x4_t a)
  {
    return _mm_rcp_ps(a);
  }

  /**
   * @brief Returns a new instance with an approximation of the reciprocal value
   * (1 / x) of the square root (Math.sqrt()) of the lane values.
   * @param a An instance of a corresponding SIMD type.
   * @return A new corresponding SIMD data type with the reciprocal (1 / x) of
   * the square root (Math.sqrt()) of the values.
   */
  static inline float32x4_t reciprocalSqrtApproximation(float32x4_t a)
  {
    return _mm_rsqrt_ps(a);
  }

}; // end of struct Float32x4

} // end of namespace SIMD
} // end of namespace BABYLON

#endif // #ifdef OPTION_ENABLE_SIMD
#endif // end of BABYLON_MATH_SIMD_FLOAT32X4_H
