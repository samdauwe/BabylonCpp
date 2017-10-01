#ifndef OIMO_MATH_DISTANCE_3d_H
#define OIMO_MATH_DISTANCE_3d_H

#include <oimo/math/vec3.h>
#include <oimo/oimo_api.h>

namespace OIMO {

struct OIMO_SHARED_EXPORT Distance3d {
  static float GetDistance3d(const Vec3& p1, const Vec3& p2);
}; // end of struct Distance3d

} // end of namespace OIMO

#endif // end of OIMO_MATH_DISTANCE_3d_H
