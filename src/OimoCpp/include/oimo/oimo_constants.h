#ifndef OIMO_CONSTANTS_H
#define OIMO_CONSTANTS_H

namespace OIMO {

namespace Math {

/** Constants **/
// Euler's e and related
const float E       = 2.71828182845904523536f;  // Euler's constant e
const float LOG2E   = 1.44269504088896340736f;  // log2(e)
const float LOG10E  = 0.434294481903251827651f; // log10(e)
const float LN2     = 0.693147180559945309417f; // ln(2)
const float LN10    = 2.30258509299404568402f;  // ln(10)
const float PI      = 3.14159265358979323846f;  // pi
const float PI2     = 6.28318530717958647692f;  // pi * 2
const float PI_2    = 1.57079632679489661923f;  // pi/2
const float PI_4    = 0.785398163397448309616f; // pi/4
const float SQRT2   = 1.41421356237309504880f;  // sqrt(2)
const float SQRT2_2 = 0.707106781186547524401f; // sqrt(2)/2

/** Conversions **/
const float DEGTORAD = 0.0174532925199432957f; //  PI / 180
const float RADTODEG = 57.295779513082320876f; // 180 / PI

} // end of namespace Math

} // end of namespace OIMO

#endif // end of OIMO_CONSTANTS_H
