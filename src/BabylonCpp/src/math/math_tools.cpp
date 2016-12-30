#include <babylon/math/math_tools.h>

namespace BABYLON {

const float MathTools::ToGammaSpace  = 1.f / 2.2f;
const float MathTools::ToLinearSpace = 2.2f;
const float MathTools::Epsilon       = 0.001f;

bool MathTools::WithinEpsilon(float a, float b, float epsilon)
{
  const float num = a - b;
  return -epsilon <= num && num <= epsilon;
}

std::string MathTools::ToHex(int i)
{
  std::ostringstream stream;
  stream << std::setfill('0') << std::setw(2) << std::hex << i;
  return stream.str();
}

// Returns -1 when value is a negative number and
// +1 when value is a positive number.
int MathTools::Sign(float x)
{
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

float MathTools::Signf(float x)
{
  return (x > 0.f) ? 1.f : ((x < 0.f) ? -1.f : 0.f);
}

float MathTools::Clamp(float value, float min, float max)
{
  return std::min(max, std::max(min, value));
}

} // end of namespace BABYLON
