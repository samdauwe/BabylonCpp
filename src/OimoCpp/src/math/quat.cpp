#include <oimo/math/quat.h>

#include <cmath>
#include <sstream>

#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

Quat::Quat(float _s, float _x, float _y, float _z) : s{_s}, x{_x}, y{_y}, z{_z}
{
}

Quat::Quat(const Quat& q) : s{q.s}, x{q.x}, y{q.y}, z{q.z}
{
}

Quat::Quat(Quat&& q)
{
  *this = std::move(q);
}

Quat::~Quat()
{
}

Quat& Quat::operator=(const Quat& q)
{
  if (&q != this) {
    s = q.s;
    x = q.x;
    y = q.y;
    z = q.z;
  }

  return *this;
}

Quat& Quat::operator=(Quat&& q)
{
  if (&q != this) {
    std::swap(s, q.s);
    std::swap(x, q.x);
    std::swap(y, q.y);
    std::swap(z, q.z);
  }

  return *this;
}

Quat Quat::clone() const
{
  return Quat(*this);
}

Quat* Quat::cloneToNewObject() const
{
  return new Quat(*this);
}

std::ostream& operator<<(std::ostream& os, const Quat& q)
{
  os << "Quat[" << q.s << ", (" << q.x << ", " << q.y << ", " << q.z << ")]";
  return os;
}

Quat& Quat::set(float _s, float _x, float _y, float _z)
{
  x = _x;
  y = _y;
  z = _z;
  s = _s;
  return *this;
}

Quat& Quat::setFromRotationMatrix(const Mat33& mtx)
{
  // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
  // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)

  const std::array<float, 9>& te = mtx.elements;

  float m11 = te[0], m12 = te[1], m13 = te[2];
  float m21 = te[3], m22 = te[4], m23 = te[5];
  float m31 = te[6], m32 = te[7], m33 = te[8];

  float trace = m11 + m22 + m33;
  float w     = 0.f;

  if (trace > 0.f) {

    w = 0.5f / std::sqrt(trace + 1.f);

    s = 0.25f / w;
    x = (m32 - m23) * w;
    y = (m13 - m31) * w;
    z = (m21 - m12) * w;
  }
  else if (m11 > m22 && m11 > m33) {

    w = 2.f * std::sqrt(1.f + m11 - m22 - m33);

    s = (m32 - m23) / w;
    x = 0.25f * w;
    y = (m12 + m21) / w;
    z = (m13 + m31) / w;
  }
  else if (m22 > m33) {

    w = 2.f * std::sqrt(1.f + m22 - m11 - m33);

    s = (m13 - m31) / w;
    x = (m12 + m21) / w;
    y = 0.25f * w;
    z = (m23 + m32) / w;
  }
  else {

    w = 2.f * std::sqrt(1.f + m33 - m11 - m22);

    s = (m21 - m12) / w;
    x = (m13 + m31) / w;
    y = (m23 + m32) / w;
    z = 0.25f * w;
  }

  return *this;
}

Quat& Quat::init(float _s, float _x, float _y, float _z)
{
  s = _s;
  x = _x;
  y = _y;
  z = _z;
  return *this;
}

Quat& Quat::add(const Quat& q1, const Quat& q2)
{
  s = q1.s + q2.s;
  x = q1.x + q2.x;
  y = q1.y + q2.y;
  z = q1.z + q2.z;
  return *this;
}

Quat& Quat::addTime(const Vec3& v, float t)
{
  const float _t = t * 0.5f;

  float ns = (-v.x * x - v.y * y - v.z * z) * _t;
  float nx = (v.x * s + v.y * z - v.z * y) * _t;
  float ny = (-v.x * z + v.y * s + v.z * x) * _t;
  float nz = (v.x * y - v.y * x + v.z * s) * _t;

  float qs = s + ns;
  float qx = x + nx;
  float qy = y + ny;
  float qz = z + nz;

  const float _s = 1.f / std::sqrt(qs * qs + qx * qx + qy * qy + qz * qz);

  s = qs * _s;
  x = qx * _s;
  y = qy * _s;
  z = qz * _s;

  return *this;
}

Quat& Quat::sub(const Quat& q1, const Quat& q2)
{
  s = q1.s - q2.s;
  x = q1.x - q2.x;
  y = q1.y - q2.y;
  z = q1.z - q2.z;
  return *this;
}

Quat& Quat::scale(const Quat& q, float _s)
{
  s = q.s * _s;
  x = q.x * _s;
  y = q.y * _s;
  z = q.z * _s;
  return *this;
}

Quat& Quat::mul(const Quat& q1, const Quat& q2)
{
  s = q1.s * q2.s - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  x = q1.s * q2.x + q1.x * q2.s + q1.y * q2.z - q1.z * q2.y;
  y = q1.s * q2.y - q1.x * q2.z + q1.y * q2.s + q1.z * q2.x;
  z = q1.s * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.s;
  return *this;
}

Quat& Quat::arc(const Vec3& v1, const Vec3& v2)
{
  float d = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  if (floats_are_equal(d, -1.f)) {
    float x2 = v1.y * v1.x - v1.z * v1.z;
    float y2 = -v1.z * v1.y - v1.x * v1.x;
    float z2 = v1.x * v1.z + v1.y * v1.y;

    d = 1.f / std::sqrt(x2 * x2 + y2 * y2 + z2 * z2);
    s = 0.f;
    x = x2 * d;
    y = y2 * d;
    z = z2 * d;
  }
  else {
    s = std::sqrt((1.f + d) * 0.5f);
    d = 0.5f / s;
    x = (v1.y * v2.z - v1.z * v2.y) * d;
    y = (v1.z * v2.x - v1.x * v2.z) * d;
    z = (v1.x * v2.y - v1.y * v2.x) * d;
  }
  return *this;
}

Quat& Quat::normalize(const Quat& q)
{
  float len = std::sqrt(q.s * q.s + q.x * q.x + q.y * q.y + q.z * q.z);
  if (len > 0.f) {
    len = 1.f / len;
  }
  s = q.s * len;
  x = q.x * len;
  y = q.y * len;
  z = q.z * len;
  return *this;
}

Quat& Quat::invert(const Quat& q)
{
  s = q.s;
  x = -q.x;
  y = -q.y;
  z = -q.z;
  return *this;
}

float Quat::length() const
{
  return std::sqrt(s * s + x * x + y * y + z * z);
}

Quat& Quat::copy(const Quat& q)
{
  s = q.s;
  x = q.x;
  y = q.y;
  z = q.z;
  return *this;
}

bool Quat::testDiff(const Quat& q)
{
  if (!floats_are_equal(s, q.s) || !floats_are_equal(x, q.x)
      || !floats_are_equal(y, q.y) || !floats_are_equal(z, q.z)) {
    return true;
  }
  else {
    return false;
  }
}

std::string Quat::toString() const
{
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

} // end of namespace OIMO
