#include <babylon/math/scalar.h>

namespace BABYLON {

float Scalar::Lerp(float start, float end, float amount)
{
  return start + ((end - start) * amount);
}

float Scalar::Hermite(float value1, float tangent1, float value2,
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

} // end of namespace BABYLON
