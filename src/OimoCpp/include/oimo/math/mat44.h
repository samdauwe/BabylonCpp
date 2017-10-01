#ifndef OIMO_MATH_MAT44_H
#define OIMO_MATH_MAT44_H

#include <array>

#include <oimo/oimo_api.h>

namespace OIMO {

class OIMO_SHARED_EXPORT Mat44 {

public:
  Mat44(float e00 = 1.f, float e01 = 0.f, float e02 = 0.f, float e03 = 0.f, //
        float e10 = 0.f, float e11 = 1.f, float e12 = 0.f, float e13 = 0.f, //
        float e20 = 0.f, float e21 = 0.f, float e22 = 1.f, float e23 = 0.f, //
        float e30 = 0.f, float e31 = 0.f, float e32 = 0.f, float e33 = 1.f);
  ~Mat44();

  Mat44&
  init(float e00 = 1.f, float e01 = 0.f, float e02 = 0.f, float e03 = 0.f, //
       float e10 = 0.f, float e11 = 1.f, float e12 = 0.f, float e13 = 0.f, //
       float e20 = 0.f, float e21 = 0.f, float e22 = 1.f, float e23 = 0.f, //
       float e30 = 0.f, float e31 = 0.f, float e32 = 0.f, float e33 = 1.f);

public:
  std::array<float, 16> elements;

}; // end of class Mat44

} // end of namespace OIMO

#endif // end of OIMO_MATH_MAT44_H
