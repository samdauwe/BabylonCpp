#include <oimo/math/mat44.h>

namespace OIMO {

Mat44::Mat44(float e00, float e01, float e02, float e03, //
             float e10, float e11, float e12, float e13, //
             float e20, float e21, float e22, float e23, //
             float e30, float e31, float e32, float e33)
{
  elements[0]  = e00;
  elements[4]  = e01;
  elements[8]  = e02;
  elements[12] = e03;
  elements[1]  = e10;
  elements[5]  = e11;
  elements[9]  = e12;
  elements[13] = e13;
  elements[2]  = e20;
  elements[6]  = e21;
  elements[10] = e22;
  elements[14] = e23;
  elements[3]  = e30;
  elements[7]  = e31;
  elements[11] = e32;
  elements[15] = e33;
}

Mat44::~Mat44()
{
}

Mat44& Mat44::init(float e00, float e01, float e02, float e03, //
                   float e10, float e11, float e12, float e13, //
                   float e20, float e21, float e22, float e23, //
                   float e30, float e31, float e32, float e33)
{
  elements[0]  = e00;
  elements[4]  = e01;
  elements[8]  = e02;
  elements[12] = e03;
  elements[1]  = e10;
  elements[5]  = e11;
  elements[9]  = e12;
  elements[13] = e13;
  elements[2]  = e20;
  elements[6]  = e21;
  elements[10] = e22;
  elements[14] = e23;
  elements[3]  = e30;
  elements[7]  = e31;
  elements[11] = e32;
  elements[15] = e33;

  return *this;
}

} // end of namespace OIMO
