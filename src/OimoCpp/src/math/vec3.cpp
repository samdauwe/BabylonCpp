#include <oimo/math/vec3.h>

#include <cmath>
#include <sstream>

#include <oimo/math/mat33.h>
#include <oimo/math/quat.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

Vec3::Vec3(float _x, float _y, float _z) : x{_x}, y{_y}, z{_z}
{
}

Vec3::Vec3(const Vec3& v) : x{v.x}, y{v.y}, z{v.z}
{
}

Vec3::Vec3(Vec3&& v)
{
  *this = std::move(v);
}

Vec3::~Vec3()
{
}

Vec3& Vec3::operator=(const Vec3& v)
{
  if (&v != this) {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  return *this;
}

Vec3& Vec3::operator=(Vec3&& v)
{
  if (&v != this) {
    std::swap(x, v.x);
    std::swap(y, v.y);
    std::swap(z, v.z);
  }

  return *this;
}

Vec3 Vec3::clone() const
{
  return Vec3(*this);
}

Vec3* Vec3::cloneToNewObject() const
{
  return new Vec3(*this);
}

std::ostream& operator<<(std::ostream& os, const Vec3& v)
{
  os << "Vec3[" << v.x << ", " << v.y << ", " << v.z << "]";
  return os;
}

Vec3& Vec3::init(float _x, float _y, float _z)
{
  x = _x;
  y = _y;
  z = _z;
  return *this;
}

Vec3& Vec3::set(float _x, float _y, float _z)
{
  x = _x;
  y = _y;
  z = _z;
  return *this;
}

Vec3& Vec3::add(const Vec3& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

Vec3& Vec3::add(const Vec3& v1, const Vec3& v2)
{
  x = v1.x + v2.x;
  y = v1.y + v2.y;
  z = v1.z + v2.z;
  return *this;
}

Vec3& Vec3::addEqual(const Vec3& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

Vec3& Vec3::addTime(const Vec3& v, float t)
{
  x += v.x * t;
  y += v.y * t;
  z += v.z * t;
  return *this;
}

Vec3& Vec3::sub(const Vec3& v1, const Vec3& v2)
{
  x = v1.x - v2.x;
  y = v1.y - v2.y;
  z = v1.z - v2.z;
  return *this;
}

Vec3& Vec3::subEqual(const Vec3& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

Vec3& Vec3::addScale(const Vec3& v, float s)
{
  x += v.x * s;
  y += v.y * s;
  z += v.z * s;
  return *this;
}

Vec3& Vec3::scale(const Quat& q, float s)
{
  x = q.x * s;
  y = q.y * s;
  z = q.z * s;
  return *this;
}

Vec3& Vec3::scale(const Vec3& v, float s)
{
  x = v.x * s;
  y = v.y * s;
  z = v.z * s;
  return *this;
}

Vec3& Vec3::scaleEqual(float s)
{
  x *= s;
  y *= s;
  z *= s;
  return *this;
}

Vec3& Vec3::cross(const Vec3& v)
{
  float _x = x, _y = y, _z = z;

  x = _y * v.z - _z * v.y;
  y = _z * v.x - _x * v.z;
  z = _x * v.y - _y * v.x;
  return *this;
}

Vec3& Vec3::cross(const Vec3& v1, const Vec3& v2)
{
  x = v1.y * v2.z - v1.z * v2.y;
  y = v1.z * v2.x - v1.x * v2.z;
  z = v1.x * v2.y - v1.y * v2.x;
  return *this;
}

Vec3& Vec3::mul(const Vec3& o, const Vec3& v, const Mat33& m)
{
  const std::array<float, 9>& te = m.elements;

  x = o.x + v.x * te[0] + v.y * te[1] + v.z * te[2];
  y = o.y + v.x * te[3] + v.y * te[4] + v.z * te[5];
  z = o.z + v.x * te[6] + v.y * te[7] + v.z * te[8];

  return *this;
}

Vec3& Vec3::mulMat(const Mat33 m, const Vec3& v)
{
  const std::array<float, 9>& te = m.elements;

  x = te[0] * v.x + te[1] * v.y + te[2] * v.z;
  y = te[3] * v.x + te[4] * v.y + te[5] * v.z;
  z = te[6] * v.x + te[7] * v.y + te[8] * v.z;

  return *this;
}

Vec3& Vec3::normalize(const Vec3& v)
{
  float l = v.x * v.x + v.y * v.y + v.z * v.z;
  if (l > 0.f) {
    l = 1.f / std::sqrt(l);
    x = v.x * l;
    y = v.y * l;
    z = v.z * l;
  }
  return *this;
}

Vec3& Vec3::invert(const Vec3& v)
{
  x = -v.x;
  y = -v.y;
  z = -v.z;
  return *this;
}

Vec3& Vec3::negate()
{
  x = -x;
  y = -y;
  z = -z;

  return *this;
}

float Vec3::dot(const Vec3& v) const
{
  return x * v.x + y * v.y + z * v.z;
}

float Vec3::lengthSq() const
{
  return x * x + y * y + z * z;
}

float Vec3::length() const
{
  return std::sqrt(x * x + y * y + z * z);
}

Vec3& Vec3::copy(const Vec3& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  return *this;
}

Vec3& Vec3::applyQuaternion(const Quat& q)
{
  float qx = q.x;
  float qy = q.y;
  float qz = q.z;
  float qs = q.s;

  // calculate quat * vector

  float ix = qs * x + qy * z - qz * y;
  float iy = qs * y + qz * x - qx * z;
  float iz = qs * z + qx * y - qy * x;
  float iw = -qx * x - qy * y - qz * z;

  // calculate result * inverse quat

  x = ix * qs + iw * -qx + iy * -qz - iz * -qy;
  y = iy * qs + iw * -qy + iz * -qx - ix * -qz;
  z = iz * qs + iw * -qz + ix * -qy - iy * -qx;

  return *this;
}

bool Vec3::testZero() const
{
  if (!floats_are_equal(x, 0.f) || !floats_are_equal(y, 0.f)
      || !floats_are_equal(z, 0.f)) {
    return true;
  }
  else {
    return false;
  }
}

bool Vec3::testDiff(const Vec3& v) const
{
  if (!floats_are_equal(x, v.x) || !floats_are_equal(y, v.y)
      || !floats_are_equal(z, v.z)) {
    return true;
  }
  else {
    return false;
  }
}

bool Vec3::equals(const Vec3& v) const
{
  return (floats_are_equal(x, v.x) && floats_are_equal(y, v.y)
          && floats_are_equal(z, v.z));
}

std::string Vec3::toString() const
{
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

Vec3& Vec3::multiplyScalar(float scalar)
{
  if (std::isfinite(scalar)) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
  }
  else {
    x = 0.f;
    y = 0.f;
    z = 0.f;
  }

  return *this;
}

Vec3& Vec3::divideScalar(float scalar)
{
  return multiplyScalar(1 / scalar);
}

Vec3& Vec3::norm()
{
  return divideScalar(length());
}

float Vec3::angleTo(const Vec3& v) const
{
  float theta = dot(v) / (std::sqrt(lengthSq() * v.lengthSq()));

  // clamp, to handle numerical problems
  return std::acos(clamp(theta, -1.f, 1.f));
}

} // end of namespace OIMO
