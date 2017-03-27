#ifndef BABYLON_MATH_MATH_TOOLS_H
#define BABYLON_MATH_MATH_TOOLS_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace MathTools {

static const float ToGammaSpace  = 1.f / 2.2f;
static const float ToLinearSpace = 2.2f;
static const float Epsilon       = 0.001f;

/**
 * @brief Returns if the absolute difference between a and b is lower than
 * epsilon (default = 1.401298E-45)
 */
template <typename T>
constexpr bool WithinEpsilon(T a, T b, T epsilon = 1.401298E-45f)
{
  return std::abs(a - b) <= epsilon;
}

/**
 * @brief Returns the upper case translation of the number i to hexadecimal.
 */
inline std::string ToHex(int i)
{
  std::ostringstream stream;
  stream << std::setfill('0') << std::setw(2) << std::hex << i;
  return stream.str();
}

/**
 * @brief Returns -1 if value is negative and +1 is value is positive.
 * Returns the 0 if it's equal to zero.
 */
template <typename T>
constexpr int Sign(T x)
{
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

/**
 * @brief Returns -1 if value is negative and +1 is value is positive.
 * Returns the 0 if it's equal to zero.
 */
template <typename T>
constexpr float Signf(T x)
{
  return (x > 0.f) ? 1.f : ((x < 0.f) ? -1.f : 0.f);
}

/**
 * @brief Returns the value itself if it's between min and max.
 * Returns min if the value is lower than min.
 * Returns max if the value is greater than max.
 */
template <typename T>
constexpr T Clamp(T value, T min, T max)
{
  return std::min(max, std::max(min, value));
}

constexpr int IMul(int a, int b)
{
  // the shift by 0 fixes the sign on the high part
  // the final |0 converts the unsigned value into a signed value
  return (
    (((a & 0xffff) * (b & 0xffff)) + (((((a >> 16) & 0xffff) * (b & 0xffff)
                                        + (a & 0xffff) * ((b >> 16) & 0xffff))
                                       << 16)
                                      >> 0))
    | 0);
}

} // end of namespace MathTools
} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATH_TOOLS_H
