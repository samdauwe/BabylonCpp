#include <oimo/math/mat33.h>

#include <cmath>
#include <iomanip> // std::setprecision
#include <sstream>

#include <oimo/math/quat.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

Mat33::Mat33(float e00, float e01, float e02, //
             float e10, float e11, float e12, //
             float e20, float e21, float e22)
{
  elements[0] = e00;
  elements[1] = e01;
  elements[2] = e02;
  elements[3] = e10;
  elements[4] = e11;
  elements[5] = e12;
  elements[6] = e20;
  elements[7] = e21;
  elements[8] = e22;
}

Mat33::Mat33(const Mat33& m)
{
  elements[0] = m.elements[0];
  elements[1] = m.elements[1];
  elements[2] = m.elements[2];
  elements[3] = m.elements[3];
  elements[4] = m.elements[4];
  elements[5] = m.elements[5];
  elements[6] = m.elements[6];
  elements[7] = m.elements[7];
  elements[8] = m.elements[8];
}

Mat33::Mat33(Mat33&& m)
{
  *this = std::move(m);
}

Mat33::~Mat33()
{
}

Mat33& Mat33::operator=(const Mat33& m)
{
  if (&m != this) {
    elements[0] = m.elements[0];
    elements[1] = m.elements[1];
    elements[2] = m.elements[2];
    elements[3] = m.elements[3];
    elements[4] = m.elements[4];
    elements[5] = m.elements[5];
    elements[6] = m.elements[6];
    elements[7] = m.elements[7];
    elements[8] = m.elements[8];
  }

  return *this;
}

Mat33& Mat33::operator=(Mat33&& m)
{
  if (&m != this) {
    std::swap(elements[0], m.elements[0]);
    std::swap(elements[1], m.elements[1]);
    std::swap(elements[2], m.elements[2]);
    std::swap(elements[3], m.elements[3]);
    std::swap(elements[4], m.elements[4]);
    std::swap(elements[5], m.elements[5]);
    std::swap(elements[6], m.elements[6]);
    std::swap(elements[7], m.elements[7]);
    std::swap(elements[8], m.elements[8]);
  }

  return *this;
}

Mat33 Mat33::clone() const
{
  return Mat33(*this);
}

Mat33* Mat33::cloneToNewObject() const
{
  return new Mat33(*this);
}

std::ostream& operator<<(std::ostream& os, const Mat33& m)
{
  const std::array<float, 9>& te = m.elements;
  os << std::setprecision(4);
  os << "Mat33|" << te[0] << ", " << te[1] << ", " << te[2] << "|\n" //
     << "     |" << te[3] << ", " << te[4] << ", " << te[5] << "|\n" //
     << "     |" << te[6] << ", " << te[7] << ", " << te[8] << "|";
  return os;
}

Mat33& Mat33::set(float e00, float e01, float e02, //
                  float e10, float e11, float e12, //
                  float e20, float e21, float e22)
{
  elements[0] = e00;
  elements[1] = e01;
  elements[2] = e02;
  elements[3] = e10;
  elements[4] = e11;
  elements[5] = e12;
  elements[6] = e20;
  elements[7] = e21;
  elements[8] = e22;
  return *this;
}

Mat33& Mat33::init(float e00, float e01, float e02, //
                   float e10, float e11, float e12, //
                   float e20, float e21, float e22)
{
  return set(e00, e01, e02, //
             e10, e11, e12, //
             e20, e21, e22);
}

Mat33& Mat33::identity()
{
  set(1.f, 0.f, 0.f, //
      0.f, 1.f, 0.f, //
      0.f, 0.f, 1.f);
  return *this;
}

Mat33& Mat33::multiply(float s)
{
  elements[0] *= s;
  elements[1] *= s;
  elements[2] *= s;
  elements[3] *= s;
  elements[4] *= s;
  elements[5] *= s;
  elements[6] *= s;
  elements[7] *= s;
  elements[8] *= s;
  return *this;
}

Mat33& Mat33::multiplyScalar(float s)
{
  return multiply(s);
}

Mat33& Mat33::add(const Mat33& m)
{
  return addEqual(m);
}

Mat33& Mat33::add(const Mat33& m1, const Mat33& m2)
{
  elements[0] = m1.elements[0] + m2.elements[0];
  elements[1] = m1.elements[1] + m2.elements[1];
  elements[2] = m1.elements[2] + m2.elements[2];
  elements[3] = m1.elements[3] + m2.elements[3];
  elements[4] = m1.elements[4] + m2.elements[4];
  elements[5] = m1.elements[5] + m2.elements[5];
  elements[6] = m1.elements[6] + m2.elements[6];
  elements[7] = m1.elements[7] + m2.elements[7];
  elements[8] = m1.elements[8] + m2.elements[8];
  return *this;
}

Mat33& Mat33::addEqual(const Mat33& m)
{
  elements[0] += m.elements[0];
  elements[1] += m.elements[1];
  elements[2] += m.elements[2];
  elements[3] += m.elements[3];
  elements[4] += m.elements[4];
  elements[5] += m.elements[5];
  elements[6] += m.elements[6];
  elements[7] += m.elements[7];
  elements[8] += m.elements[8];
  return *this;
}

Mat33& Mat33::sub(const Mat33& m1, const Mat33& m2)
{
  elements[0] = m1.elements[0] - m2.elements[0];
  elements[1] = m1.elements[1] - m2.elements[1];
  elements[2] = m1.elements[2] - m2.elements[2];
  elements[3] = m1.elements[3] - m2.elements[3];
  elements[4] = m1.elements[4] - m2.elements[4];
  elements[5] = m1.elements[5] - m2.elements[5];
  elements[6] = m1.elements[6] - m2.elements[6];
  elements[7] = m1.elements[7] - m2.elements[7];
  elements[8] = m1.elements[8] - m2.elements[8];
  return *this;
}

Mat33& Mat33::subEqual(const Mat33& m)
{
  elements[0] -= m.elements[0];
  elements[1] -= m.elements[1];
  elements[2] -= m.elements[2];
  elements[3] -= m.elements[3];
  elements[4] -= m.elements[4];
  elements[5] -= m.elements[5];
  elements[6] -= m.elements[6];
  elements[7] -= m.elements[7];
  elements[8] -= m.elements[8];
  return *this;
}

Mat33& Mat33::scale(const Mat33& m, float s)
{
  elements[0] = m.elements[0] * s;
  elements[1] = m.elements[1] * s;
  elements[2] = m.elements[2] * s;
  elements[3] = m.elements[3] * s;
  elements[4] = m.elements[4] * s;
  elements[5] = m.elements[5] * s;
  elements[6] = m.elements[6] * s;
  elements[7] = m.elements[7] * s;
  elements[8] = m.elements[8] * s;
  return *this;
}

Mat33& Mat33::scaleEqual(float s)
{
  elements[0] *= s;
  elements[1] *= s;
  elements[2] *= s;
  elements[3] *= s;
  elements[4] *= s;
  elements[5] *= s;
  elements[6] *= s;
  elements[7] *= s;
  elements[8] *= s;
  return *this;
}

Mat33& Mat33::mul(const Mat33& m1, const Mat33& m2, bool transpose)
{
  const std::array<float, 9>& tm1 = m1.elements;
  const std::array<float, 9>& tm2
    = transpose ? m2.clone().transpose().elements : m2.elements;

  float a0 = tm1[0], a3 = tm1[3], a6 = tm1[6];
  float a1 = tm1[1], a4 = tm1[4], a7 = tm1[7];
  float a2 = tm1[2], a5 = tm1[5], a8 = tm1[8];

  float b0 = tm2[0], b3 = tm2[3], b6 = tm2[6];
  float b1 = tm2[1], b4 = tm2[4], b7 = tm2[7];
  float b2 = tm2[2], b5 = tm2[5], b8 = tm2[8];

  elements[0] = a0 * b0 + a1 * b3 + a2 * b6;
  elements[1] = a0 * b1 + a1 * b4 + a2 * b7;
  elements[2] = a0 * b2 + a1 * b5 + a2 * b8;
  elements[3] = a3 * b0 + a4 * b3 + a5 * b6;
  elements[4] = a3 * b1 + a4 * b4 + a5 * b7;
  elements[5] = a3 * b2 + a4 * b5 + a5 * b8;
  elements[6] = a6 * b0 + a7 * b3 + a8 * b6;
  elements[7] = a6 * b1 + a7 * b4 + a8 * b7;
  elements[8] = a6 * b2 + a7 * b5 + a8 * b8;
  return *this;
}

Mat33& Mat33::multiplyMatrices(const Mat33& m1, const Mat33& m2, bool transpose)
{
  return mul(m1, m2, transpose);
}

Mat33& Mat33::mulScale(const Mat33& m, float sx, float sy, float sz,
                       bool prepend)
{
  const std::array<float, 9>& tm = m.elements;
  if (prepend) {
    elements[0] = sx * tm[0];
    elements[1] = sx * tm[1];
    elements[2] = sx * tm[2];
    elements[3] = sy * tm[3];
    elements[4] = sy * tm[4];
    elements[5] = sy * tm[5];
    elements[6] = sz * tm[6];
    elements[7] = sz * tm[7];
    elements[8] = sz * tm[8];
  }
  else {
    elements[0] = tm[0] * sx;
    elements[1] = tm[1] * sy;
    elements[2] = tm[2] * sz;
    elements[3] = tm[3] * sx;
    elements[4] = tm[4] * sy;
    elements[5] = tm[5] * sz;
    elements[6] = tm[6] * sx;
    elements[7] = tm[7] * sy;
    elements[8] = tm[8] * sz;
  }
  return *this;
}

Mat33& Mat33::mulRotate(const Mat33& m, float rad, float ax, float ay, float az,
                        bool prepend)
{
  float s   = std::sin(rad);
  float c   = std::cos(rad);
  float c1  = 1.f - c;
  float r00 = ax * ax * c1 + c;
  float r01 = ax * ay * c1 - az * s;
  float r02 = ax * az * c1 + ay * s;
  float r10 = ay * ax * c1 + az * s;
  float r11 = ay * ay * c1 + c;
  float r12 = ay * az * c1 - ax * s;
  float r20 = az * ax * c1 - ay * s;
  float r21 = az * ay * c1 + ax * s;
  float r22 = az * az * c1 + c;

  const std::array<float, 9>& tm = m.elements;

  float a0 = tm[0], a3 = tm[3], a6 = tm[6];
  float a1 = tm[1], a4 = tm[4], a7 = tm[7];
  float a2 = tm[2], a5 = tm[5], a8 = tm[8];

  if (prepend) {
    elements[0] = r00 * a0 + r01 * a3 + r02 * a6;
    elements[1] = r00 * a1 + r01 * a4 + r02 * a7;
    elements[2] = r00 * a2 + r01 * a5 + r02 * a8;
    elements[3] = r10 * a0 + r11 * a3 + r12 * a6;
    elements[4] = r10 * a1 + r11 * a4 + r12 * a7;
    elements[5] = r10 * a2 + r11 * a5 + r12 * a8;
    elements[6] = r20 * a0 + r21 * a3 + r22 * a6;
    elements[7] = r20 * a1 + r21 * a4 + r22 * a7;
    elements[8] = r20 * a2 + r21 * a5 + r22 * a8;
  }
  else {
    elements[0] = a0 * r00 + a1 * r10 + a2 * r20;
    elements[1] = a0 * r01 + a1 * r11 + a2 * r21;
    elements[2] = a0 * r02 + a1 * r12 + a2 * r22;
    elements[3] = a3 * r00 + a4 * r10 + a5 * r20;
    elements[4] = a3 * r01 + a4 * r11 + a5 * r21;
    elements[5] = a3 * r02 + a4 * r12 + a5 * r22;
    elements[6] = a6 * r00 + a7 * r10 + a8 * r20;
    elements[7] = a6 * r01 + a7 * r11 + a8 * r21;
    elements[8] = a6 * r02 + a7 * r12 + a8 * r22;
  }
  return *this;
}

Mat33& Mat33::transpose()
{
  float a01 = elements[1], a02 = elements[2], a12 = elements[5];
  elements[1] = elements[3];
  elements[2] = elements[6];
  elements[3] = a01;
  elements[5] = elements[7];
  elements[6] = a02;
  elements[7] = a12;
  return *this;
}

Mat33& Mat33::transpose(const Mat33& m)
{
  elements[0] = m.elements[0];
  elements[1] = m.elements[3];
  elements[2] = m.elements[6];
  elements[3] = m.elements[1];
  elements[4] = m.elements[4];
  elements[5] = m.elements[7];
  elements[6] = m.elements[2];
  elements[7] = m.elements[5];
  elements[8] = m.elements[8];
  return *this;
}

Mat33& Mat33::setQuat(const Quat& q)
{
  const float x = q.x, y = q.y, z = q.z, w = q.w;
  const float x2 = x + x, y2 = y + y, z2 = z + z;
  const float xx = x * x2, xy = x * y2, xz = x * z2;
  const float yy = y * y2, yz = y * z2, zz = z * z2;
  const float wx = w * x2, wy = w * y2, wz = w * z2;

  elements[0] = 1.f - (yy + zz);
  elements[1] = xy - wz;
  elements[2] = xz + wy;

  elements[3] = xy + wz;
  elements[4] = 1.f - (xx + zz);
  elements[5] = yz - wx;

  elements[6] = xz - wy;
  elements[7] = yz + wx;
  elements[8] = 1.f - (xx + yy);

  return *this;
}

Mat33& Mat33::invert(const Mat33& m)
{
  const std::array<float, 9>& tm = m.elements;
  const float a00 = tm[0], a10 = tm[3], a20 = tm[6];
  const float a01 = tm[1], a11 = tm[4], a21 = tm[7];
  const float a02 = tm[2], a12 = tm[5], a22 = tm[8];
  const float b01 = a22 * a11 - a12 * a21;
  const float b11 = -a22 * a10 + a12 * a20;
  const float b21 = a21 * a10 - a11 * a20;
  float det       = a00 * b01 + a01 * b11 + a02 * b21;

  if (floats_are_equal(det, 0.f)) {
    // can't invert matrix, determinant is 0
    return identity();
  }

  det         = 1.f / det;
  elements[0] = b01 * det;
  elements[1] = (-a22 * a01 + a02 * a21) * det;
  elements[2] = (a12 * a01 - a02 * a11) * det;
  elements[3] = b11 * det;
  elements[4] = (a22 * a00 - a02 * a20) * det;
  elements[5] = (-a12 * a00 + a02 * a10) * det;
  elements[6] = b21 * det;
  elements[7] = (-a21 * a00 + a01 * a20) * det;
  elements[8] = (a11 * a00 - a01 * a10) * det;
  return *this;
}

Mat33& Mat33::addOffset(float m, const Vec3& v)
{
  float relX = v.x;
  float relY = v.y;
  float relZ = v.z;

  elements[0] += m * (relY * relY + relZ * relZ);
  elements[4] += m * (relX * relX + relZ * relZ);
  elements[8] += m * (relX * relX + relY * relY);
  float xy = m * relX * relY;
  float yz = m * relY * relZ;
  float zx = m * relZ * relX;
  elements[1] -= xy;
  elements[3] -= xy;
  elements[2] -= yz;
  elements[6] -= yz;
  elements[5] -= zx;
  elements[7] -= zx;
  return *this;
}

Mat33& Mat33::subOffset(float m, const Vec3& v)
{
  float relX = v.x;
  float relY = v.y;
  float relZ = v.z;

  elements[0] -= m * (relY * relY + relZ * relZ);
  elements[4] -= m * (relX * relX + relZ * relZ);
  elements[8] -= m * (relX * relX + relY * relY);
  float xy = m * relX * relY;
  float yz = m * relY * relZ;
  float zx = m * relZ * relX;
  elements[1] += xy;
  elements[3] += xy;
  elements[2] += yz;
  elements[6] += yz;
  elements[5] += zx;
  elements[7] += zx;
  return *this;
}

Mat33& Mat33::copy(const Mat33& m)
{
  set(m.elements[0], m.elements[3], m.elements[6], //
      m.elements[1], m.elements[4], m.elements[7], //
      m.elements[2], m.elements[5], m.elements[8]);
  return *this;
}

float Mat33::determinant() const
{
  float a = elements[0], b = elements[1], c = elements[2], //
    d = elements[3], e = elements[4], f = elements[5],     //
    g = elements[6], h = elements[7], i = elements[8];

  return a * e * i - a * f * h - b * d * i + b * f * g + c * d * h - c * e * g;
}

Mat33& Mat33::fromArray(const std::array<float, 9>& array)
{
  elements = array;

  return *this;
}

std::array<float, 9> Mat33::toArray() const
{
  return {{elements[0], elements[1], elements[2], //
           elements[3], elements[4], elements[5], //
           elements[6], elements[7], elements[8]}};
}

std::string Mat33::toString() const
{
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

} // end of namespace OIMO
