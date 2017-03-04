#ifndef OIMO_MATH_NATH_UTILS_H
#define OIMO_MATH_NATH_UTILS_H

#include <cmath>
#include <oimo/math/vec3.h>
#include <oimo/oimo_constants.h>

namespace OIMO {
namespace Math {

inline float acosClamp(float cos)
{
  if (cos > 1.f) {
    return 0;
  }
  else if (cos < -1.f) {
    return Math::PI;
  }
  else {
    return std::acos(cos);
  }
}

inline float distanceVector(const Vec3& v1, const Vec3& v2)
{
  float xd = v1.x - v2.x;
  float yd = v1.y - v2.y;
  float zd = v1.z - v2.z;
  return xd * xd + yd * yd + zd * zd;
}

inline float dotVectors(const Vec3& a, const Vec3& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

} // end of namespace Math
} // end of namespace OIMO

#endif // end of OIMO_MATH_NATH_UTILS_H
