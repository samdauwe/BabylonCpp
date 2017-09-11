#ifndef BABYLON_MATH_SCALAR_H
#define BABYLON_MATH_SCALAR_H

#include <babylon/babylon_global.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/random.h>
#include <babylon/core/string.h>

namespace BABYLON {
namespace Scalar {

/**
 * @brief Returns if the absolute difference between a and b is lower than
 * epsilon (default = 1.401298E-45)
 */
template <typename T>
constexpr bool WithinEpsilon(T a, T b, T epsilon = 1.401298E-45f)
{
  return ::std::abs(a - b) <= epsilon;
}

/**
 * @brief Returns the upper case translation of the number i to hexadecimal.
 */
inline std::string ToHex(int i)
{
  std::ostringstream stream;
  stream << ::std::setfill('0') << ::std::setw(2) << ::std::hex << i;
  return String::toUpperCase(stream.str());
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

/**
 * @brief Returns the value itself if it's between 0 and 1.
 */
template <typename T>
constexpr T Clamp(T value)
{
  return ::std::min(static_cast<T>(1), ::std::max(static_cast<T>(0), value));
}

/**
 * @brief Returns the log2 of value.
 */
template <typename T>
constexpr T Log2(T value)
{
  return std::log(value) * Math::LOG2E;
}

constexpr int IMul(int a, int b)
{
  // the shift by 0 fixes the sign on the high part
  // the final |0 converts the unsigned value into a signed value
  return ((((a & 0xffff) * (b & 0xffff))
           + (((((a >> 16) & 0xffff) * (b & 0xffff)
                + (a & 0xffff) * ((b >> 16) & 0xffff))
               << 16)
              >> 0))
          | 0);
}

/**
 * @brief Loops the value, so that it is never larger than length and never
 * smaller than 0.
 *
 * This is similar to the modulo operator but it works with floating point
 * numbers.
 * For example, using 3.0 for t and 2.5 for length, the result would be 0.5.
 * With t = 5 and length = 2.5, the result would be 0.0.
 * Note, however, that the behaviour is not defined for negative numbers as it
 * is for the modulo operator
 */
inline float Repeat(float value, float length)
{
  return value - ::std::floor(value / length) * length;
}

/**
 * @brief Normalize the value between 0.0 and 1.0 using min and max values
 */
constexpr float Normalize(float value, float min, float max)
{
  return (value - min) / (max - min);
}

/**
 * @brief Denormalize the value from 0.0 and 1.0 using min and max values
 */
constexpr float Denormalize(float normalized, float min, float max)
{
  return (normalized * (max - min) + min);
}

/**
 * @brief Calculates the shortest difference between two given angles given in
 * degrees.
 */
inline float DeltaAngle(float current, float target)
{
  auto num = Scalar::Repeat(target - current, 360.f);
  if (num > 180.f) {
    num -= 360.f;
  }
  return num;
}

/**
 * @brief PingPongs the value t, so that it is never larger than length and
 * never smaller than 0.
 *
 * The returned value will move back and forth between 0 and length
 */
inline float PingPong(float tx, float length)
{
  const auto t = Scalar::Repeat(tx, length * 2.f);
  return length - ::std::abs(t - length);
}

/**
 * @brief Interpolates between min and max with smoothing at the limits.
 *
 * This function interpolates between min and max in a similar way to Lerp.
 * However, the interpolation will gradually speed up
 * from the start and slow down toward the end. This is useful for creating
 * natural-looking animation, fading and other transitions.
 */
constexpr float SmoothStep(float from, float to, float tx)
{
  auto t = Scalar::Clamp(tx);
  t      = -2.f * t * t * t + 3.f * t * t;
  return to * t + from * (1.f - t);
}

/**
 * @brief Moves a value current towards target.
 *
 * This is essentially the same as Mathf.Lerp but instead the function will
 * ensure that the speed never exceeds maxDelta.
 * Negative values of maxDelta pushes the value away from target.
 */
inline float MoveTowards(float current, float target, float maxDelta)
{
  auto result = 0.f;
  if (::std::abs(target - current) <= maxDelta) {
    result = target;
  }
  else {
    result = current + Scalar::Sign(target - current) * maxDelta;
  }
  return result;
}

/**
 * @brief Same as MoveTowards but makes sure the values interpolate correctly
 * when they wrap around 360 degrees.
 *
 * Variables current and target are assumed to be in degrees. For optimization
 * reasons, negative values of maxDelta
 *  are not supported and may cause oscillation. To push current away from a
 * target angle, add 180 to that angle instead.
 */
inline float MoveTowardsAngle(float current, float target, float maxDelta)
{
  const auto num = Scalar::DeltaAngle(current, target);
  auto result    = 0.f;
  if (-maxDelta < num && num < maxDelta) {
    result = target;
  }
  else {
    target = current + num;
    result = Scalar::MoveTowards(current, target, maxDelta);
  }
  return result;
}

/**
 * @brief Creates a new scalar with values linearly interpolated of "amount"
 * between the start scalar and the end scalar.
 */
template <typename T>
constexpr T Lerp(T start, T end, T amount)
{
  return start + ((end - start) * amount);
}

/**
 * @brief Same as Lerp but makes sure the values interpolate correctly when they
 * wrap around 360 degrees.
 */
inline float LerpAngle(float start, float end, float amount)
{
  auto num = Scalar::Repeat(end - start, 360.f);
  if (num > 180.f) {
    num -= 360.f;
  }
  return start + num * Scalar::Clamp(amount);
}

/**
 * @brief Calculates the linear parameter t that produces the interpolant value
 * within the range [a, b].
 */
constexpr float InverseLerp(float a, float b, float value)
{
  auto result = 0.f;
  if (!stl_util::almost_equal(a, b)) {
    result = Scalar::Clamp((value - a) / (b - a));
  }
  else {
    result = 0.f;
  }
  return result;
}

/**
 * @brief Returns a new scalar located for "amount" (float) on the Hermite
 * spline defined by the scalars "value1", "value3", "tangent1", "tangent2".
 */
constexpr float Hermite(float value1, float tangent1, float value2,
                        float tangent2, float amount)
{
  const float squared = amount * amount;
  const float cubed   = amount * squared;
  const float part1   = ((2.f * cubed) - (3.f * squared)) + 1.f;
  const float part2   = (-2.f * cubed) + (3.f * squared);
  const float part3   = (cubed - (2.f * squared)) + amount;
  const float part4   = cubed - squared;

  return (((value1 * part1) + (value2 * part2)) + (tangent1 * part3))
         + (tangent2 * part4);
}

/**
 * @brief Returns a random float number between and min and max values
 */
inline float RandomRange(float min, float max)
{
  if (stl_util::almost_equal(min, max)) {
    return min;
  }
  return ((Math::random() * (max - min)) + min);
}

/**
 * @brief This function returns percentage of a number in a given range.
 *
 * RangeToPercent(40,20,60) will return 0.5 (50%)
 * RangeToPercent(34,0,100) will return 0.34 (34%)
 */
constexpr float RangeToPercent(float number, float min, float max)
{
  return ((number - min) / (max - min));
}

/**
 * @brief This function returns number that corresponds to the percentage in a
 * given range.
 *
 * PercentToRange(0.34,0,100) will return 34.
 */
constexpr float PercentToRange(float percent, float min, float max)
{
  return ((max - min) * percent + min);
}

} // end of namespace Scalar
} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SCALAR_H
