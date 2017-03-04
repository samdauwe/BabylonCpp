#include <oimo/math/quat.h>

#include <cmath>
#include <sstream>

#include <oimo/math/mat33.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_constants.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

Quat::Quat(float _x, float _y, float _z, float _w) : x{_x}, y{_y}, z{_z}, w{_w}
{
}

Quat::Quat(const Quat& q) : x{q.x}, y{q.y}, z{q.z}, w{q.w}
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
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
  }

  return *this;
}

Quat& Quat::operator=(Quat&& q)
{
  if (&q != this) {
    std::swap(x, q.x);
    std::swap(y, q.y);
    std::swap(z, q.z);
    std::swap(w, q.w);
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
  os << "Quat[" << q.x << ", (" << q.y << ", " << q.z << ", " << q.w << ")]";
  return os;
}

Quat& Quat::set(float _x, float _y, float _z, float _w)
{
  x = _x;
  y = _y;
  z = _z;
  w = _w;
  return *this;
}

Quat& Quat::setFromEuler(float x, float y, float z)
{
  const float c1 = std::cos(x * 0.5f);
  const float c2 = std::cos(y * 0.5f);
  const float c3 = std::cos(z * 0.5f);
  const float s1 = std::sin(x * 0.5f);
  const float s2 = std::sin(y * 0.5f);
  const float s3 = std::sin(z * 0.5f);

  // XYZ
  x = s1 * c2 * c3 + c1 * s2 * s3;
  y = c1 * s2 * c3 - s1 * c2 * s3;
  z = c1 * c2 * s3 + s1 * s2 * c3;
  w = c1 * c2 * c3 - s1 * s2 * s3;

  return *this;
}

Quat& Quat::setFromAxis(Vec3& axis, float rad)
{
  axis.normalize();
  rad     = rad * 0.5f;
  float s = std::sin(rad);
  x       = s * axis.x;
  y       = s * axis.y;
  z       = s * axis.z;
  w       = std::cos(rad);
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
  float s     = 0.f;

  if (trace > 0.f) {

    s = 0.5f / std::sqrt(trace + 1.f);

    w = 0.25f / s;
    x = (m32 - m23) * s;
    y = (m13 - m31) * s;
    z = (m21 - m12) * s;
  }
  else if (m11 > m22 && m11 > m33) {

    s = 2.f * std::sqrt(1.f + m11 - m22 - m33);

    w = (m32 - m23) / s;
    x = 0.25f * s;
    y = (m12 + m21) / s;
    z = (m13 + m31) / s;
  }
  else if (m22 > m33) {

    s = 2.f * std::sqrt(1.f + m22 - m11 - m33);

    w = (m13 - m31) / s;
    x = (m12 + m21) / s;
    y = 0.25f * s;
    z = (m23 + m32) / s;
  }
  else {

    s = 2.f * std::sqrt(1.f + m33 - m11 - m22);

    w = (m21 - m12) / s;
    x = (m13 + m31) / s;
    y = (m23 + m32) / s;
    z = 0.25f * s;
  }

  return *this;
}

Quat& Quat::init(float _x, float _y, float _z, float _w)
{
  x = _x;
  y = _y;
  z = _z;
  w = _w;
  return *this;
}

Quat& Quat::add(const Quat& q1, const Quat& q2)
{
  x = q1.x + q2.x;
  y = q1.y + q2.y;
  z = q1.z + q2.z;
  w = q1.w + q2.w;
  return *this;
}

Quat& Quat::addTime(const Vec3& v, float t)
{
  float ax = v.x, ay = v.y, az = v.z;
  float qw = w, qx = x, qy = y, qz = z;
  t *= 0.5f;
  x += t * (ax * qw + ay * qz - az * qy);
  y += t * (ay * qw + az * qx - ax * qz);
  z += t * (az * qw + ax * qy - ay * qx);
  w += t * (-ax * qx - ay * qy - az * qz);
  normalize();
  return *this;
}

Quat& Quat::scale(const Quat& q, float _s)
{
  x = q.x * _s;
  y = q.y * _s;
  z = q.z * _s;
  w = q.w * _s;
  return *this;
}

Quat& Quat::mul(const Quat& a)
{
  return mul(*this, a);
}

Quat& Quat::mul(const Quat& a, const Quat& b)
{
  const float qax = a.x, qay = a.y, qaz = a.z, qaw = a.w;
  const float qbx = b.x, qby = b.y, qbz = b.z, qbw = b.w;

  x = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
  y = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
  z = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
  w = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;
  return *this;
}

Quat& Quat::multiply(const Quat& a)
{
  return mul(a);
}

Quat& Quat::multiplyQuaternions(const Quat& a, const Quat& b)
{
  return mul(a, b);
}

Quat& Quat::setFromUnitVectors(const Vec3& v1, const Vec3& v2)
{
  Vec3 vx;
  float r = v1.dot(v2) + 1.f;

  if (r < Math::EPZ2) {
    r = 0.f;
    if (std::abs(v1.x) > std::abs(v1.z)) {
      vx.set(-v1.y, v1.x, 0.f);
    }
    else {
      vx.set(0, -v1.z, v1.y);
    }
  }
  else {
    vx.crossVectors(v1, v2);
  }

  x = vx.x;
  y = vx.y;
  z = vx.z;
  w = r;

  return normalize();
}

Quat& Quat::arc(const Vec3& v1, const Vec3& v2)
{
  float x1 = v1.x, y1 = v1.y, z1 = v1.z;
  float x2 = v2.x, y2 = v2.y, z2 = v2.z;
  float d = x1 * x2 + y1 * y2 + z1 * z2;
  if (floats_are_equal(d, -1.f)) {
    x2 = y1 * x1 - z1 * z1;
    y2 = -z1 * y1 - x1 * x1;
    z2 = x1 * z1 + y1 * y1;
    d  = 1.f / std::sqrt(x2 * x2 + y2 * y2 + z2 * z2);
    w  = 0.f;
    x  = x2 * d;
    y  = y2 * d;
    z  = z2 * d;
  }
  else {
    float cx = y1 * z2 - z1 * y2;
    float cy = z1 * x2 - x1 * z2;
    float cz = x1 * y2 - y1 * x2;
    w        = std::sqrt((1.f + d) * 0.5f);
    d        = 0.5f / w;
    x        = cx * d;
    y        = cy * d;
    z        = cz * d;
  }
  return *this;
}

Quat& Quat::normalize()
{
  float l = length();
  if (floats_are_equal(l, 0.f)) {
    set(0.f, 0.f, 0.f, 1.f);
  }
  else {
    l = 1.f / l;
    x = x * l;
    y = y * l;
    z = z * l;
    w = w * l;
  }
  return *this;
}

Quat& Quat::normalize(const Quat& q)
{
  float l = q.length();
  if (l > 0.f) {
    l = 1.f / l;
  }
  x = q.x * l;
  y = q.y * l;
  z = q.z * l;
  w = q.w * l;
  return *this;
}

Quat& Quat::inverse()
{
  return conjugate().normalize();
}

Quat& Quat::invert(const Quat& q)
{
  x = q.x;
  y = q.y;
  z = q.z;
  w = q.w;
  conjugate().normalize();
  return *this;
}

Quat& Quat::conjugate()
{
  x *= -1.f;
  y *= -1.f;
  z *= -1.f;
  return *this;
}

float Quat::length() const
{
  return std::sqrt(x * x + y * y + z * z + w * w);
}

float Quat::lengthSq() const
{
  return x * x + y * y + z * z + w * w;
}

Quat& Quat::copy(const Quat& q)
{
  x = q.x;
  y = q.y;
  z = q.z;
  w = q.w;
  return *this;
}

bool Quat::testDiff(const Quat& q) const
{
  return equals(q) ? false : true;
}

bool Quat::equals(const Quat& q) const
{
  return (floats_are_equal(x, q.x) && floats_are_equal(y, q.y)
          && floats_are_equal(z, q.z) && floats_are_equal(w, q.w));
}

std::string Quat::toString() const
{
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

void Quat::toArray(std::vector<float> array, size_t offset)
{
  array[offset]     = x;
  array[offset + 1] = y;
  array[offset + 2] = z;
  array[offset + 3] = w;
}

Quat& Quat::fromArray(const std::vector<float> array, size_t offset)
{
  set(array[offset], array[offset + 1], array[offset + 2], array[offset + 3]);
  return *this;
}

} // end of namespace OIMO
