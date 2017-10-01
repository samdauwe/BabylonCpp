#ifndef OIMO_MATH_AXIS_H
#define OIMO_MATH_AXIS_H

#include <oimo/oimo_api.h>

namespace OIMO {

struct OIMO_SHARED_EXPORT Axis {
  float angle;
  float x;
  float y;
  float z;
}; // end of struct Axis

} // end of namespace OIMO

#endif // end of OIMO_MATH_AXIS_H