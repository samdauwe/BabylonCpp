#include <oimo/math/mat_rotation.h>

#include <cmath>

#include <oimo/math/axis.h>
#include <oimo/math/euler.h>
#include <oimo/math/mat33.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

Axis MatRotation::EulerToAxis(float ox, float oy, float oz)
{
  float c1    = std::cos(oy * 0.5f); // heading
  float s1    = std::sin(oy * 0.5f);
  float c2    = std::cos(oz * 0.5f); // altitude
  float s2    = std::sin(oz * 0.5f);
  float c3    = std::cos(ox * 0.5f); // bank
  float s3    = std::sin(ox * 0.5f);
  float c1c2  = c1 * c2;
  float s1s2  = s1 * s2;
  float w     = c1c2 * c3 - s1s2 * s3;
  float x     = c1c2 * s3 + s1s2 * c3;
  float y     = s1 * c2 * c3 + c1 * s2 * s3;
  float z     = c1 * s2 * c3 - s1 * c2 * s3;
  float angle = 2.f * std::acos(w);
  float norm  = x * x + y * y + z * z;
  if (norm < 0.001f) {
    x = 1.f;
    y = z = 0.f;
  }
  else {
    norm = std::sqrt(norm);
    x /= norm;
    y /= norm;
    z /= norm;
  }
  return {angle, x, y, z};
}

Mat33 MatRotation::EulerToMatrix(float ox, float oy, float oz)
{
  float ch = std::cos(oy); // heading
  float sh = std::sin(oy);
  float ca = std::cos(oz); // altitude
  float sa = std::sin(oz);
  float cb = std::cos(ox); // bank
  float sb = std::sin(ox);
  Mat33 mtx;

  std::array<float, 9>& te = mtx.elements;
  te[0] = ch * ca;
  te[1] = sh * sb - ch * sa * cb;
  te[2] = ch * sa * sb + sh * cb;
  te[3] = sa;
  te[4] = ca * cb;
  te[5] = -ca * sb;
  te[6] = -sh * ca;
  te[7] = sh * sa * cb + ch * sb;
  te[8] = -sh * sa * sb + ch * cb;
  return mtx;
}

Euler MatRotation::MatrixToEuler(const Mat33& mtx)
{
  const std::array<float, 9>& te = mtx.elements;
  float x, y, z;
  if (te[3] > 0.998f) { // singularity at north pole
    y = std::atan2(te[2], te[8]);
    z = Math::PI_2;
    x = 0;
  }
  else if (te[3] < -0.998f) { // singularity at south pole
    y = std::atan2(te[2], te[8]);
    z = -Math::PI_2;
    x = 0;
  }
  else {
    y = std::atan2(-te[6], te[0]);
    x = std::atan2(-te[5], te[4]);
    z = std::asin(te[3]);
  }
  return Euler(x, y, z);
}

float MatRotation::UnwrapDegrees(float r)
{
  float rem = r - std::floor(r / 360.f);
  if (rem > 180)
    rem -= 360;
  if (rem < -180)
    rem += 360;
  return rem;
}

float MatRotation::UnwrapRadian(float r)
{
  float rem = r - std::floor(r / Math::PI2);
  if (rem > Math::PI)
    rem -= Math::PI2;
  if (rem < -Math::PI)
    rem += Math::PI2;
  return rem;
}

} // end of namespace OIMO
