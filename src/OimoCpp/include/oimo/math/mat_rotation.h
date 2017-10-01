#ifndef OIMO_MATH_MAT_ROTATION_H
#define OIMO_MATH_MAT_ROTATION_H

#include <oimo/oimo_api.h>

namespace OIMO {

struct Axis;
class Euler;
class Mat33;

struct OIMO_SHARED_EXPORT MatRotation {
  static Axis EulerToAxis(float ox, float oy, float oz);
  static Mat33 EulerToMatrix(float ox, float oy, float oz);
  static Euler MatrixToEuler(const Mat33& m);
  static float UnwrapDegrees(float r);
  static float UnwrapRadian(float r);
}; // end of struct MatRotation

} // end of namespace OIMO

#endif // end of OIMO_MATH_MAT_ROTATION_H
