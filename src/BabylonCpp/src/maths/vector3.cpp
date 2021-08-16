#include <babylon/maths/vector3.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/maths/axis.h>
#include <babylon/maths/math_tmp.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/plane.h>
#include <babylon/maths/quaternion.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/vector3.h>
#include <babylon/maths/viewport.h>

namespace BABYLON {

const Vector3 Vector3::_UpReadOnly                 = Vector3::Up();
const Vector3 Vector3::_LeftHandedForwardReadOnly  = Vector3::Forward(false);
const Vector3 Vector3::_RightHandedForwardReadOnly = Vector3::Forward(true);
const Vector3 Vector3::_RightReadOnly              = Vector3::Right();
const Vector3 Vector3::_ZeroReadOnly               = Vector3::Zero();

Vector3::Vector3() : x{0.f}, y{0.f}, z{0.f}
{
}

Vector3::Vector3(float ix, float iy, float iz) : x{ix}, y{iy}, z{iz}
{
}

Vector3::Vector3(const Vector3& otherVector) = default;

Vector3::Vector3(Vector3&& otherVector) = default;

Vector3::~Vector3() = default;

Vector3& Vector3::operator=(const Vector3& otherVector) = default;

Vector3& Vector3::operator=(Vector3&& otherVector) = default;

Vector3 Vector3::copy() const
{
  return Vector3(*this);
}

std::unique_ptr<Vector3> Vector3::clone() const
{
  return std::make_unique<Vector3>(*this);
}

std::string Vector3::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

const char* Vector3::getClassName() const
{
  return "Vector3";
}

size_t Vector3::getHashCode() const
{
  float tmp = ((x * 0.397f) + y * 3.97f) + z;
  return stl_util::to_bitset(tmp).to_ullong();
}

/** Operators **/
Float32Array Vector3::asArray() const
{
  Float32Array result;
  toArray(result, 0);

  return result;
}

const Vector3& Vector3::toArray(Float32Array& array, unsigned int index) const
{
  if (array.size() < index + 3) {
    array.resize(index + 3);
  }

  array[index]     = x;
  array[index + 1] = y;
  array[index + 2] = z;

  return *this;
}

Vector3& Vector3::fromArray(const Float32Array& array, unsigned int index)
{
  Vector3::FromArrayToRef(array, index, *this);
  return *this;
}

Quaternion Vector3::toQuaternion() const
{
  return Quaternion::RotationYawPitchRoll(y, x, z);
}

Vector3& Vector3::addInPlace(const Vector3& otherVector)
{
  return addInPlaceFromFloats(otherVector.x, otherVector.y, otherVector.z);
}

Vector3& Vector3::addInPlaceFromFloats(float ix, float iy, float iz)
{
  x += ix;
  y += iy;
  z += iz;

  return *this;
}

Vector3 Vector3::add(const Vector3& otherVector) const
{
  return Vector3(x + otherVector.x, y + otherVector.y, z + otherVector.z);
}

Vector3& Vector3::addToRef(const Vector3& otherVector, Vector3& result)
{
  return result.copyFromFloats(x + otherVector.x, y + otherVector.y, z + otherVector.z);
}

const Vector3& Vector3::addToRef(const Vector3& otherVector, Vector3& result) const
{
  return result.copyFromFloats(x + otherVector.x, y + otherVector.y, z + otherVector.z);
}

Vector3& Vector3::subtractInPlace(const Vector3& otherVector)
{
  x -= otherVector.x;
  y -= otherVector.y;
  z -= otherVector.z;

  return *this;
}

Vector3 Vector3::subtract(const Vector3& otherVector) const
{
  return Vector3(x - otherVector.x, y - otherVector.y, z - otherVector.z);
}

Vector3& Vector3::subtractToRef(const Vector3& otherVector, Vector3& result)
{
  return subtractFromFloatsToRef(otherVector.x, otherVector.y, otherVector.z, result);
}

const Vector3& Vector3::subtractToRef(const Vector3& otherVector, Vector3& result) const
{
  return subtractFromFloatsToRef(otherVector.x, otherVector.y, otherVector.z, result);
}

Vector3 Vector3::subtractFromFloats(float ix, float iy, float iz) const
{
  return Vector3(x - ix, y - iy, z - iz);
}

Vector3& Vector3::subtractFromFloatsToRef(float ix, float iy, float iz, Vector3& result)
{
  return result.copyFromFloats(x - ix, y - iy, z - iz);
}

const Vector3& Vector3::subtractFromFloatsToRef(float ix, float iy, float iz, Vector3& result) const
{
  return result.copyFromFloats(x - ix, y - iy, z - iz);
}

Vector3 Vector3::negate() const
{
  return Vector3(-x, -y, -z);
}

Vector3& Vector3::negateInPlace()
{
  x *= -1.f;
  y *= -1.f;
  z *= -1.f;
  return *this;
}

Vector3 Vector3::negateToRef(Vector3& result)
{
  return result.copyFromFloats(x * -1.f, y * -1.f, z * -1.f);
}

Vector3& Vector3::scaleInPlace(float iscale)
{
  x *= iscale;
  y *= iscale;
  z *= iscale;

  return *this;
}

Vector3 Vector3::scale(float iscale) const
{
  return Vector3(x * iscale, y * iscale, z * iscale);
}

const Vector3& Vector3::scaleToRef(int iscale, Vector3& result) const
{
  const auto scalef = static_cast<float>(iscale);

  return result.copyFromFloats(x * scalef, y * scalef, z * scalef);
}

const Vector3& Vector3::scaleToRef(float iscale, Vector3& result) const
{
  return result.copyFromFloats(x * iscale, y * iscale, z * iscale);
}

const Vector3& Vector3::scaleAndAddToRef(int iscale, Vector3& result) const
{
  const auto scalef = static_cast<float>(iscale);

  return result.addInPlaceFromFloats(x * scalef, y * scalef, z * scalef);
}

const Vector3& Vector3::scaleAndAddToRef(float iscale, Vector3& result) const
{
  return result.addInPlaceFromFloats(x * iscale, y * iscale, z * iscale);
}

Vector3 Vector3::projectOnPlane(const Plane& plane, const Vector3& origin) const
{
  auto result = Vector3::Zero();

  projectOnPlaneToRef(plane, origin, result);

  return result;
}

void Vector3::projectOnPlaneToRef(const Plane& plane, const Vector3& origin, Vector3& result) const
{
  const auto& n = plane.normal;
  const auto d  = plane.d;

  auto& V = MathTmp::Vector3Array[0];

  // ray direction
  subtractToRef(origin, V);

  V.normalize();

  const auto denom = Vector3::Dot(V, n);
  const auto t     = -(Vector3::Dot(origin, n) + d) / denom;

  // P = P0 + t*V
  const auto scaledV = V.scaleInPlace(t);
  origin.addToRef(scaledV, result);
}

bool Vector3::equals(const Vector3& otherVector) const
{
  return stl_util::almost_equal(x, otherVector.x) && stl_util::almost_equal(y, otherVector.y)
         && stl_util::almost_equal(z, otherVector.z);
}

bool Vector3::equalsWithEpsilon(const Vector3& otherVector, float epsilon) const
{
  return Scalar::WithinEpsilon(x, otherVector.x, epsilon)
         && Scalar::WithinEpsilon(y, otherVector.y, epsilon)
         && Scalar::WithinEpsilon(z, otherVector.z, epsilon);
}

bool Vector3::equalsToFloats(float ix, float iy, float iz) const
{
  return stl_util::almost_equal(x, ix) && stl_util::almost_equal(y, iy)
         && stl_util::almost_equal(z, iz);
}

Vector3& Vector3::multiplyInPlace(const Vector3& otherVector)
{
  x *= otherVector.x;
  y *= otherVector.y;
  z *= otherVector.z;

  return *this;
}

Vector3 Vector3::multiply(const Vector3& otherVector) const
{
  return multiplyByFloats(otherVector.x, otherVector.y, otherVector.z);
}

const Vector3& Vector3::multiplyToRef(const Vector3& otherVector, Vector3& result) const
{
  return result.copyFromFloats(x * otherVector.x, y * otherVector.y, z * otherVector.z);
}

Vector3 Vector3::multiplyByFloats(float ix, float iy, float iz) const
{
  return Vector3(x * ix, y * iy, z * iz);
}

Vector3 Vector3::divide(const Vector3& otherVector) const
{
  return Vector3(x / otherVector.x, y / otherVector.y, z / otherVector.z);
}

Vector3& Vector3::divideToRef(const Vector3& otherVector, Vector3& result) const
{
  return result.copyFromFloats(x / otherVector.x, y / otherVector.y, z / otherVector.z);
}

Vector3& Vector3::divideInPlace(const Vector3& otherVector)
{
  return divideToRef(otherVector, *this);
}

Vector3& Vector3::minimizeInPlace(const Vector3& other)
{
  return minimizeInPlaceFromFloats(other.x, other.y, other.z);
}

Vector3& Vector3::maximizeInPlace(const Vector3& other)
{
  return maximizeInPlaceFromFloats(other.x, other.y, other.z);
}

Vector3& Vector3::minimizeInPlaceFromFloats(float ix, float iy, float iz)
{
  if (ix < x) {
    x = ix;
  }
  if (iy < y) {
    y = iy;
  }
  if (iz < z) {
    z = iz;
  }
  return *this;
}

Vector3& Vector3::maximizeInPlaceFromFloats(float ix, float iy, float iz)
{
  if (ix > x) {
    x = ix;
  }
  if (iy > y) {
    y = iy;
  }
  if (iz > z) {
    z = iz;
  }
  return *this;
}

bool Vector3::isNonUniformWithinEpsilon(float epsilon)
{
  const auto absX = std::abs(x);
  const auto absY = std::abs(y);
  if (!Scalar::WithinEpsilon(absX, absY, epsilon)) {
    return true;
  }

  const auto absZ = std::abs(z);
  if (!Scalar::WithinEpsilon(absX, absZ, epsilon)) {
    return true;
  }

  if (!Scalar::WithinEpsilon(absY, absZ, epsilon)) {
    return true;
  }

  return false;
}

bool Vector3::isNonUniform() const
{
  const auto absX = std::abs(x);
  const auto absY = std::abs(y);
  if (!stl_util::almost_equal(absX, absY)) {
    return true;
  }

  const auto absZ = std::abs(z);
  if (!stl_util::almost_equal(absX, absZ)) {
    return true;
  }

  return false;
}

Vector3 Vector3::floor() const
{
  return Vector3(std::floor(x), std::floor(y), std::floor(z));
}

Vector3 Vector3::fract() const
{
  return Vector3(x - std::floor(x), y - std::floor(y), z - std::floor(z));
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Vector3& vector)
{
  os << "{\"X\":" << vector.x << ",\"Y\":" << vector.y << ",\"Z\":" << vector.z << "}";
  return os;
}

Vector3 Vector3::operator+(const Vector3& otherVector) const
{
  return add(otherVector);
}

Vector3& Vector3::operator+=(const Vector3& otherVector)
{
  return addInPlace(otherVector);
}

Vector3 Vector3::operator-(const Vector3& otherVector) const
{
  return subtract(otherVector);
}

Vector3& Vector3::operator-=(const Vector3& otherVector)
{
  return subtractInPlace(otherVector);
}

Vector3 Vector3::operator-() const
{
  return negate();
}

Vector3 Vector3::operator*(float scaleVal) const
{
  return scale(scaleVal);
}

Vector3& Vector3::operator*=(float scaleVal)
{
  return scaleInPlace(scaleVal);
}

Vector3 Vector3::operator*(const Vector3& otherVector) const
{
  return multiply(otherVector);
}

Vector3& Vector3::operator*=(const Vector3& otherVector)
{
  return multiplyInPlace(otherVector);
}

Vector3 Vector3::operator/(float value) const
{
  return Vector3(x / value, y / value, z / value);
}

Vector3 Vector3::operator/(const Vector3& otherVector) const
{
  return divide(otherVector);
}

Vector3& Vector3::operator/=(float scaleVal)
{
  return scaleInPlace(1.f / scaleVal);
}

Vector3& Vector3::operator/=(const Vector3& otherVector)
{
  return divideInPlace(otherVector);
}

bool Vector3::operator==(const Vector3& otherVector) const
{
  return equals(otherVector);
}

bool Vector3::operator!=(const Vector3& otherVector) const
{
  return !(operator==(otherVector));
}

const float& Vector3::operator[](unsigned int index) const
{
  const unsigned int _index = index % 3;
  if (_index == 1) {
    return x;
  }
  else if (_index == 2) {
    return y;
  }
  else {
    return z;
  }
}

/** Properties **/
float Vector3::length() const
{
  return std::sqrt(x * x + y * y + z * z);
}

float Vector3::lengthSquared() const
{
  return (x * x + y * y + z * z);
}

/** Methods **/
Vector3& Vector3::normalize()
{
  return normalizeFromLength(length());
}

Vector3& Vector3::reorderInPlace(std::string order)
{
  order = StringTools::toLowerCase(order);
  if (order == "xyz") {
    return *this;
  }

  const Float32Array vector3Array{x, y, z};
  std::unordered_map<char, unsigned int> orderMapping{
    {'x', 0},
    {'y', 1},
    {'z', 2},
  };

  x = vector3Array[orderMapping[order[0]]];
  y = vector3Array[orderMapping[order[1]]];
  z = vector3Array[orderMapping[order[2]]];

  return *this;
}

Vector3& Vector3::rotateByQuaternionToRef(const Quaternion& quaternion, Vector3& result)
{
  quaternion.toRotationMatrix(MathTmp::MatrixArray[0]);
  Vector3::TransformCoordinatesToRef(*this, MathTmp::MatrixArray[0], result);
  return result;
}

Vector3& Vector3::rotateByQuaternionAroundPointToRef(const Quaternion& quaternion,
                                                     const Vector3& point, Vector3& result)
{
  subtractToRef(point, MathTmp::Vector3Array[0]);
  MathTmp::Vector3Array[0].rotateByQuaternionToRef(quaternion, MathTmp::Vector3Array[0]);
  point.addToRef(MathTmp::Vector3Array[0], result);
  return result;
}

Vector3 Vector3::cross(const Vector3& other)
{
  return Vector3::Cross(*this, other);
}

Vector3& Vector3::normalizeFromLength(float len)
{
  if (len == 0.f || len == 1.f) {
    return *this;
  }

  return scaleInPlace(1.f / len);
}

Vector3 Vector3::normalizeToNew() const
{
  Vector3 normalized(0.f, 0.f, 0.f);
  normalizeToRef(normalized);
  return normalized;
}

Vector3 Vector3::normalizeToRef(Vector3& reference) const
{
  const auto len = length();
  if (stl_util::almost_equal(len, 0.f) || stl_util::almost_equal(len, 1.f)) {
    return reference.copyFromFloats(x, y, z);
  }

  return scaleToRef(1.f / len, reference);
}

Vector3& Vector3::copyFrom(const Vector3& source)
{
  return copyFromFloats(source.x, source.y, source.z);
}

Vector3& Vector3::copyFromFloats(float ix, float iy, float iz)
{
  x = ix;
  y = iy;
  z = iz;

  return *this;
}

Vector3& Vector3::set(float ix, float iy, float iz)
{
  return copyFromFloats(ix, iy, iz);
}

Vector3& Vector3::setAll(float v)
{
  x = y = z = v;
  return *this;
}

/** Statics **/
float Vector3::GetClipFactor(const Vector3& vector0, const Vector3& vector1, const Vector3& axis,
                             float size)
{
  const float d0 = Vector3::Dot(vector0, axis) - size;
  const float d1 = Vector3::Dot(vector1, axis) - size;

  const float s = d0 / (d0 - d1);

  return s;
}

float Vector3::GetAngleBetweenVectors(const Vector3& vector0, const Vector3& vector1,
                                      const Vector3& normal)
{
  const auto v0    = vector0.normalizeToRef(MathTmp::Vector3Array[1]);
  const auto v1    = vector1.normalizeToRef(MathTmp::Vector3Array[2]);
  const auto dot   = Vector3::Dot(v0, v1);
  const auto angle = std::acos(dot);
  auto& n          = MathTmp::Vector3Array[3];
  Vector3::CrossToRef(v0, v1, n);
  if (Vector3::Dot(n, normal) > 0.f) {
    return std::isnan(angle) ? 0.f : angle;
  }
  return std::isnan(angle) ? -Math::PI : -std::acos(dot);
}

float Vector3::GetAngleBetweenVectorsOnPlane(const Vector3& vector0, const Vector3& vector1,
                                             const Vector3& normal)
{
  MathTmp::Vector3Array[0].copyFrom(vector0);
  auto& v0 = MathTmp::Vector3Array[0];
  MathTmp::Vector3Array[1].copyFrom(vector1);
  auto& v1 = MathTmp::Vector3Array[1];
  MathTmp::Vector3Array[2].copyFrom(normal);
  auto& vNormal = MathTmp::Vector3Array[2];
  auto& right   = MathTmp::Vector3Array[3];
  auto& forward = MathTmp::Vector3Array[4];

  v0.normalize();
  v1.normalize();
  vNormal.normalize();

  Vector3::CrossToRef(vNormal, v0, right);
  Vector3::CrossToRef(right, vNormal, forward);

  const auto angle = std::atan2(Vector3::Dot(v1, right), Vector3::Dot(v1, forward));

  return Scalar::NormalizeRadians(angle);
}

void Vector3::SlerpToRef(const Vector3& vector0, const Vector3& vector1, float slerp,
                         Vector3& result)
{
  slerp              = Scalar::Clamp(slerp, 0.f, 1.f);
  auto& vector0Dir   = MathTmp::Vector3Array[0];
  auto& vector1Dir   = MathTmp::Vector3Array[1];
  auto vector0Length = 0.f;
  auto vector1Length = 0.f;

  vector0Dir.copyFrom(vector0);
  vector0Length = vector0Dir.length();
  vector0Dir.normalizeFromLength(vector0Length);

  vector1Dir.copyFrom(vector1);
  vector1Length = vector1Dir.length();
  vector1Dir.normalizeFromLength(vector1Length);

  const auto dot = Vector3::Dot(vector0Dir, vector1Dir);

  auto scale0 = 0.f;
  auto scale1 = 0.f;

  if (dot < 1.f - Math::Epsilon) {
    const auto omega  = std::acos(dot);
    const auto invSin = 1.f / std::sin(omega);
    scale0            = std::sin((1.f - slerp) * omega) * invSin;
    scale1            = std::sin(slerp * omega) * invSin;
  }
  else {
    // Use linear interpolation
    scale0 = 1.f - slerp;
    scale1 = slerp;
  }

  vector0Dir.scaleInPlace(scale0);
  vector1Dir.scaleInPlace(scale1);
  result.copyFrom(vector0Dir).addInPlace(vector1Dir);
  result.scaleInPlace(Scalar::Lerp(vector0Length, vector1Length, slerp));
}

void Vector3::SmoothToRef(const Vector3& source, const Vector3& goal, float deltaTime,
                          float lerpTime, Vector3& result)
{
  Vector3::SlerpToRef(source, goal, lerpTime == 0.f ? 1.f : deltaTime / lerpTime, result);
}

Vector3 Vector3::FromArray(const Float32Array& array, unsigned int offset)
{
  return Vector3(array[offset], array[offset + 1], array[offset + 2]);
}

void Vector3::FromArrayToRef(const Float32Array& array, unsigned int offset, Vector3& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
}

void Vector3::FromArrayToRef(const std::array<float, 16>& array, unsigned int offset,
                             Vector3& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
}

void Vector3::FromFloatArrayToRef(const Float32Array& array, unsigned int offset, Vector3& result)
{
  return Vector3::FromArrayToRef(array, offset, result);
}

void Vector3::FromFloatArrayToRef(const std::array<float, 16>& array, unsigned int offset,
                                  Vector3& result)
{
  return Vector3::FromArrayToRef(array, offset, result);
}

void Vector3::FromFloatsToRef(float ix, float iy, float iz, Vector3& result)
{
  result.copyFromFloats(ix, iy, iz);
}

Vector3 Vector3::Zero()
{
  return Vector3(0.f, 0.f, 0.f);
}

Vector3 Vector3::One()
{
  return Vector3(1.f, 1.f, 1.f);
}

Vector3 Vector3::Up()
{
  return Vector3(0.f, 1.f, 0.f);
}

Vector3 Vector3::UpReadOnly()
{
  return Vector3::_UpReadOnly;
}

Vector3 Vector3::RightReadOnly()
{
  return Vector3::_RightReadOnly;
}

Vector3 Vector3::LeftHandedForwardReadOnly()
{
  return Vector3::_LeftHandedForwardReadOnly;
}
Vector3 Vector3::RightHandedForwardReadOnly()
{
  return Vector3::_RightHandedForwardReadOnly;
}

Vector3 Vector3::ZeroReadOnly()
{
  return Vector3::_ZeroReadOnly;
}

Vector3 Vector3::Down()
{
  return Vector3(0.f, -1.f, 0.f);
}

Vector3 Vector3::Forward(bool rightHandedSystem)
{
  return Vector3(0.f, 0.f, (rightHandedSystem ? -1.f : 1.f));
}

Vector3 Vector3::Backward(bool rightHandedSystem)
{
  return Vector3(0.f, 0.f, (rightHandedSystem ? 1.f : -1.f));
}

Vector3 Vector3::Right()
{
  return Vector3(1.f, 0.f, 0.f);
}

Vector3 Vector3::Left()
{
  return Vector3(-1.f, 0.f, 0.f);
}

Vector3 Vector3::TransformCoordinates(const Vector3& vector, const Matrix& transformation)
{
  Vector3 result = Vector3::Zero();
  Vector3::TransformCoordinatesToRef(vector, transformation, result);
  return result;
}

void Vector3::TransformCoordinatesToRef(const Vector3& vector, const Matrix& transformation,
                                        Vector3& result)
{
  Vector3::TransformCoordinatesFromFloatsToRef(vector.x, vector.y, vector.z, transformation,
                                               result);
}

void Vector3::TransformCoordinatesFromFloatsToRef(float x, float y, float z,
                                                  const Matrix& transformation, Vector3& result)
{
  const auto& m = transformation.m();
  const auto rx = x * m[0] + y * m[4] + z * m[8] + m[12];
  const auto ry = x * m[1] + y * m[5] + z * m[9] + m[13];
  const auto rz = x * m[2] + y * m[6] + z * m[10] + m[14];
  const auto rw = 1 / (x * m[3] + y * m[7] + z * m[11] + m[15]);

  result.x = rx * rw;
  result.y = ry * rw;
  result.z = rz * rw;
}

Vector3 Vector3::TransformNormal(const Vector3& vector, const Matrix& transformation)
{
  Vector3 result = Vector3::Zero();
  Vector3::TransformNormalToRef(vector, transformation, result);
  return result;
}

void Vector3::TransformNormalToRef(const Vector3& vector, const Matrix& transformation,
                                   Vector3& result)
{
  TransformNormalFromFloatsToRef(vector.x, vector.y, vector.z, transformation, result);
}

void Vector3::TransformNormalFromFloatsToRef(float x, float y, float z,
                                             const Matrix& transformation, Vector3& result)
{
  const auto& m = transformation.m();
  result.x      = x * m[0] + y * m[4] + z * m[8];
  result.y      = x * m[1] + y * m[5] + z * m[9];
  result.z      = x * m[2] + y * m[6] + z * m[10];
}

Vector3 Vector3::CatmullRom(const Vector3& value1, const Vector3& value2, const Vector3& value3,
                            const Vector3& value4, float amount)
{
  const float squared = amount * amount;
  const float cubed   = amount * squared;

  const float x
    = 0.5f
      * ((((2.f * value2.x) + ((-value1.x + value3.x) * amount))
          + (((((2.f * value1.x) - (5.f * value2.x)) + (4.f * value3.x)) - value4.x) * squared))
         + ((((-value1.x + (3.f * value2.x)) - (3.f * value3.x)) + value4.x) * cubed));

  const float y
    = 0.5f
      * ((((2.f * value2.y) + ((-value1.y + value3.y) * amount))
          + (((((2.f * value1.y) - (5.f * value2.y)) + (4.f * value3.y)) - value4.y) * squared))
         + ((((-value1.y + (3.f * value2.y)) - (3.f * value3.y)) + value4.y) * cubed));

  const float z
    = 0.5f
      * ((((2.f * value2.z) + ((-value1.z + value3.z) * amount))
          + (((((2.f * value1.z) - (5.f * value2.z)) + (4.f * value3.z)) - value4.z) * squared))
         + ((((-value1.z + (3.f * value2.z)) - (3.f * value3.z)) + value4.z) * cubed));

  return Vector3(x, y, z);
}

Vector3 Vector3::Clamp(const Vector3& value, const Vector3& min, const Vector3& max)
{
  Vector3 v;
  Vector3::ClampToRef(value, min, max, v);
  return v;
}

void Vector3::ClampToRef(const Vector3& value, const Vector3& min, const Vector3& max,
                         Vector3& result)
{
  float x = value.x;

  x = (x > max.x) ? max.x : x;
  x = (x < min.x) ? min.x : x;

  float y = value.y;

  y = (y > max.y) ? max.y : y;
  y = (y < min.y) ? min.y : y;

  float z = value.z;

  z = (z > max.z) ? max.z : z;
  z = (z < min.z) ? min.z : z;

  result.copyFromFloats(x, y, z);
}

void Vector3::CheckExtends(Vector3& v, Vector3& min, Vector3& max)
{
  min.minimizeInPlace(v);
  max.maximizeInPlace(v);
}

Vector3 Vector3::Hermite(const Vector3& value1, const Vector3& tangent1, const Vector3& value2,
                         const Vector3& tangent2, float amount)
{
  const float squared = amount * amount;
  const float cubed   = amount * squared;
  const float part1   = ((2.f * cubed) - (3.f * squared)) + 1.f;
  const float part2   = (-2.f * cubed) + (3.f * squared);
  const float part3   = (cubed - (2.f * squared)) + amount;
  const float part4   = cubed - squared;

  const float x
    = (((value1.x * part1) + (value2.x * part2)) + (tangent1.x * part3)) + (tangent2.x * part4);
  const float y
    = (((value1.y * part1) + (value2.y * part2)) + (tangent1.y * part3)) + (tangent2.y * part4);
  const float z
    = (((value1.z * part1) + (value2.z * part2)) + (tangent1.z * part3)) + (tangent2.z * part4);

  return Vector3(x, y, z);
}

Vector3 Vector3::Hermite1stDerivative(const Vector3& value1, const Vector3& tangent1,
                                      const Vector3& value2, const Vector3& tangent2, float time)
{
  auto result = Vector3::Zero();

  Hermite1stDerivativeToRef(value1, tangent1, value2, tangent2, time, result);

  return result;
}

void Vector3::Hermite1stDerivativeToRef(const Vector3& value1, const Vector3& tangent1,
                                        const Vector3& value2, const Vector3& tangent2, float time,
                                        Vector3& result)
{
  const auto t2 = time * time;

  result.x = (t2 - time) * 6.f * value1.x + (3.f * t2 - 4.f * time + 1.f) * tangent1.x
             + (-t2 + time) * 6.f * value2.x + (3.f * t2 - 2.f * time) * tangent2.x;
  result.y = (t2 - time) * 6.f * value1.y + (3.f * t2 - 4.f * time + 1.f) * tangent1.y
             + (-t2 + time) * 6.f * value2.y + (3.f * t2 - 2.f * time) * tangent2.y;
  result.z = (t2 - time) * 6.f * value1.z + (3.f * t2 - 4.f * time + 1.f) * tangent1.z
             + (-t2 + time) * 6.f * value2.z + (3.f * t2 - 2.f * time) * tangent2.z;
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float amount)
{
  Vector3 result(0.f, 0.f, 0.f);
  Vector3::LerpToRef(start, end, amount, result);
  return result;
}

void Vector3::LerpToRef(const Vector3& start, const Vector3& end, float amount, Vector3& result)
{
  result.x = start.x + ((end.x - start.x) * amount);
  result.y = start.y + ((end.y - start.y) * amount);
  result.z = start.z + ((end.z - start.z) * amount);
}

float Vector3::Dot(const Vector3& left, const Vector3& right)
{
  return (left.x * right.x + left.y * right.y + left.z * right.z);
}

Vector3 Vector3::Cross(const Vector3& left, const Vector3& right)
{
  auto result = Vector3::Zero();
  Vector3::CrossToRef(left, right, result);
  return result;
}

void Vector3::CrossToRef(const Vector3& left, const Vector3& right, Vector3& result)
{
  const auto x = left.y * right.z - left.z * right.y;
  const auto y = left.z * right.x - left.x * right.z;
  const auto z = left.x * right.y - left.y * right.x;

  result.copyFromFloats(x, y, z);
}

Vector3 Vector3::Normalize(const Vector3& vector)
{
  auto result = Vector3::Zero();
  Vector3::NormalizeToRef(vector, result);
  return result;
}

void Vector3::NormalizeToRef(const Vector3& vector, Vector3& result)
{
  vector.normalizeToRef(result);
}

Vector3 Vector3::Project(const Vector3& vector, Matrix& world, Matrix& transform,
                         const Viewport& viewport)
{
  Vector3 result;
  Vector3::ProjectToRef(vector, world, transform, viewport, result);
  return result;
}

Vector3& Vector3::ProjectToRef(const Vector3& vector, Matrix& world, Matrix& transform,
                               const Viewport& viewport, Vector3& result)
{
  const auto cw = static_cast<float>(viewport.width);
  const auto ch = static_cast<float>(viewport.height);
  const auto cx = static_cast<float>(viewport.x);
  const auto cy = static_cast<float>(viewport.y);

  auto& viewportMatrix = MathTmp::MatrixArray[1];

  Matrix::FromValuesToRef(cw / 2.f, 0.f, 0.f, 0.f,  //
                          0.f, -ch / 2.f, 0.f, 0.f, //
                          0.f, 0.f, 0.5f, 0.f,      //
                          cx + cw / 2.f, ch / 2.f + cy, 0.5f, 1.f, viewportMatrix);

  auto& matrix = MathTmp::MatrixArray[0];
  world.multiplyToRef(transform, matrix);
  matrix.multiplyToRef(viewportMatrix, matrix);

  Vector3::TransformCoordinatesToRef(vector, matrix, result);
  return result;
}

void Vector3::_UnprojectFromInvertedMatrixToRef(const Vector3& source, const Matrix& matrix,
                                                Vector3& result)
{
  Vector3::TransformCoordinatesToRef(source, matrix, result);
  const auto& m  = matrix.m();
  const auto num = source.x * m[3] + source.y * m[7] + source.z * m[11] + m[15];
  if (Scalar::WithinEpsilon(num, 1.f)) {
    result.scaleInPlace(1.f / num);
  }
}

Vector3 Vector3::UnprojectFromTransform(Vector3& source, float viewportWidth, float viewportHeight,
                                        Matrix& world, Matrix& transform)
{
  auto& matrix = MathTmp::MatrixArray[0];
  world.multiplyToRef(transform, matrix);
  matrix.invert();
  source.x = source.x / viewportWidth * 2.f - 1.f;
  source.y = -(source.y / viewportHeight * 2.f - 1.f);
  Vector3 vector;
  Vector3::_UnprojectFromInvertedMatrixToRef(source, matrix, vector);

  return vector;
}

Vector3 Vector3::Unproject(const Vector3& source, float viewportWidth, float viewportHeight,
                           Matrix& world, Matrix& view, Matrix& projection)
{
  auto result = Vector3::Zero();

  Vector3::UnprojectToRef(source, viewportWidth, viewportHeight, world, view, projection, result);

  return result;
}

void Vector3::UnprojectToRef(const Vector3& source, float viewportWidth, float viewportHeight,
                             Matrix& world, Matrix& view, Matrix& projection, Vector3& result)
{
  Vector3::UnprojectFloatsToRef(source.x, source.y, source.z, viewportWidth, viewportHeight, world,
                                view, projection, result);
}

void Vector3::UnprojectFloatsToRef(float sourceX, float sourceY, float sourceZ, float viewportWidth,
                                   float viewportHeight, Matrix& world, Matrix& view,
                                   Matrix& projection, Vector3& result)
{
  auto& matrix = MathTmp::MatrixArray[0];
  world.multiplyToRef(view, matrix);
  matrix.multiplyToRef(projection, matrix);
  matrix.invert();
  auto& screenSource = MathTmp::Vector3Array[0];
  screenSource.x     = sourceX / viewportWidth * 2.f - 1.f;
  screenSource.y     = -(sourceY / viewportHeight * 2.f - 1.f);
  screenSource.z     = 2.f * sourceZ - 1.f;
  Vector3::_UnprojectFromInvertedMatrixToRef(screenSource, matrix, result);
}

Vector3 Vector3::Minimize(const Vector3& left, const Vector3& right)
{
  Vector3 min = left;
  min.minimizeInPlace(right);
  return min;
}

Vector3 Vector3::Maximize(const Vector3& left, const Vector3& right)
{
  Vector3 max = left;
  max.maximizeInPlace(right);
  return max;
}

float Vector3::Distance(const Vector3& value1, const Vector3& value2)
{
  return std::sqrt(Vector3::DistanceSquared(value1, value2));
}

float Vector3::DistanceSquared(const Vector3& value1, const Vector3& value2)
{
  const float x = value1.x - value2.x;
  const float y = value1.y - value2.y;
  const float z = value1.z - value2.z;

  return (x * x) + (y * y) + (z * z);
}

float Vector3::ProjectOnTriangleToRef(const Vector3& vector, const Vector3& p0, const Vector3& p1,
                                      const Vector3& p2, Vector3& ref)
{
  auto& p1p0     = MathTmp::Vector3Array[0];
  auto& p2p0     = MathTmp::Vector3Array[1];
  auto& p2p1     = MathTmp::Vector3Array[2];
  auto& normal   = MathTmp::Vector3Array[3];
  auto& vectorp0 = MathTmp::Vector3Array[4];

  // Triangle vectors
  p1.subtractToRef(p0, p1p0);
  p2.subtractToRef(p0, p2p0);
  p2.subtractToRef(p1, p2p1);

  const auto p1p0L = p1p0.length();
  const auto p2p0L = p2p0.length();
  const auto p2p1L = p2p1.length();

  if (p1p0L < Math::Epsilon || //
      p2p0L < Math::Epsilon || //
      p2p1L < Math::Epsilon) {
    // This is a degenerate triangle. As we assume this is part of a non-degenerate mesh,
    // we will find a better intersection later.
    // Let's just return one of the extremities
    ref.copyFrom(p0);
    return Vector3::Distance(vector, p0);
  }

  // Compute normal and vector to p0
  vector.subtractToRef(p0, vectorp0);
  Vector3::CrossToRef(p1p0, p2p0, normal);
  const auto nl = normal.length();
  if (nl < Math::Epsilon) {
    // Extremities are aligned, we are back on the case of a degenerate triangle
    ref.copyFrom(p0);
    return Vector3::Distance(vector, p0);
  }
  normal.normalizeFromLength(nl);
  auto l = vectorp0.length();
  if (l < Math::Epsilon) {
    // Vector is p0
    ref.copyFrom(p0);
    return 0;
  }
  vectorp0.normalizeFromLength(l);

  // Project to "proj" that lies on the triangle plane
  const auto cosA  = Vector3::Dot(normal, vectorp0);
  auto& projVector = MathTmp::Vector3Array[5];
  auto& proj       = MathTmp::Vector3Array[6];
  projVector.copyFrom(normal).scaleInPlace(-l * cosA);
  proj.copyFrom(vector).addInPlace(projVector);

  // Compute barycentric coordinates (v0, v1 and v2 are axis from barycenter to extremities)
  auto& v0  = MathTmp::Vector3Array[4];
  auto& v1  = MathTmp::Vector3Array[5];
  auto& v2  = MathTmp::Vector3Array[7];
  auto& tmp = MathTmp::Vector3Array[8];

  v0.copyFrom(p1p0).scaleInPlace(1.f / p1p0L);
  tmp.copyFrom(p2p0).scaleInPlace(1.f / p2p0L);
  v0.addInPlace(tmp).scaleInPlace(-1.f);

  v1.copyFrom(p1p0).scaleInPlace(-1.f / p1p0L);
  tmp.copyFrom(p2p1).scaleInPlace(1.f / p2p1L);
  v1.addInPlace(tmp).scaleInPlace(-1.f);

  v2.copyFrom(p2p1).scaleInPlace(-1.f / p2p1L);
  tmp.copyFrom(p2p0).scaleInPlace(-1.f / p2p0L);
  v2.addInPlace(tmp).scaleInPlace(-1.f);

  // Determines which edge of the triangle is closest to "proj"
  auto& projP = MathTmp::Vector3Array[9];
  auto dot    = 0.f;
  auto s0 = 0.f, s1 = 0.f, s2 = 0.f;
  projP.copyFrom(proj).subtractInPlace(p0);
  Vector3::CrossToRef(v0, projP, tmp);
  dot = Vector3::Dot(tmp, normal);
  s0  = dot;

  projP.copyFrom(proj).subtractInPlace(p1);
  Vector3::CrossToRef(v1, projP, tmp);
  dot = Vector3::Dot(tmp, normal);
  s1  = dot;

  projP.copyFrom(proj).subtractInPlace(p2);
  Vector3::CrossToRef(v2, projP, tmp);
  dot = Vector3::Dot(tmp, normal);
  s2  = dot;

  auto& edge = MathTmp::Vector3Array[10];
  auto e0 = Vector3(), e1 = Vector3();
  if (s0 > 0.f && s1 < 0.f) {
    edge.copyFrom(p1p0);
    e0 = p0;
    e1 = p1;
  }
  else if (s1 > 0.f && s2 < 0.f) {
    edge.copyFrom(p2p1);
    e0 = p1;
    e1 = p2;
  }
  else {
    edge.copyFrom(p2p0).scaleInPlace(-1);
    e0 = p2;
    e1 = p0;
  }

  // Determines if "proj" lies inside the triangle
  auto& tmp2 = MathTmp::Vector3Array[9];
  auto& tmp3 = MathTmp::Vector3Array[4];
  e0.subtractToRef(proj, tmp);
  e1.subtractToRef(proj, tmp2);
  Vector3::CrossToRef(tmp, tmp2, tmp3);
  const auto isOutside = Vector3::Dot(tmp3, normal) < 0;

  // If inside, we already found the projected point, "proj"
  if (!isOutside) {
    ref.copyFrom(proj);
    return std::abs(l * cosA);
  }

  // If outside, we find "triProj", the closest point from "proj" on the closest edge
  auto& r = MathTmp::Vector3Array[5];
  Vector3::CrossToRef(edge, tmp3, r);
  r.normalize();
  auto& e0proj = MathTmp::Vector3Array[9];
  e0proj.copyFrom(e0).subtractInPlace(proj);
  const auto e0projL = e0proj.length();
  if (e0projL < Math::Epsilon) {
    // Proj is e0
    ref.copyFrom(e0);
    return Vector3::Distance(vector, e0);
  }
  e0proj.normalizeFromLength(e0projL);
  const auto cosG = Vector3::Dot(r, e0proj);
  auto& triProj   = MathTmp::Vector3Array[7];
  triProj.copyFrom(proj).addInPlace(r.scaleInPlace(e0projL * cosG));

  // Now we clamp "triProj" so it lies between e0 and e1
  tmp.copyFrom(triProj).subtractInPlace(e0);
  l = edge.length();
  edge.normalizeFromLength(l);
  auto t = Vector3::Dot(tmp, edge) / std::max(l, Math::Epsilon);
  t      = Scalar::Clamp(t, 0.f, 1.f);
  triProj.copyFrom(e0).addInPlace(edge.scaleInPlace(t * l));
  ref.copyFrom(triProj);

  return Vector3::Distance(vector, triProj);
}

Vector3 Vector3::Center(const Vector3& value1, const Vector3& value2)
{
  Vector3 center = Vector3::Zero();
  Vector3::CenterToRef(value1, value2, center);
  return center;
}

Vector3& Vector3::CenterToRef(const Vector3& value1, const Vector3& value2, Vector3& ref)
{
  return ref.copyFromFloats((value1.x + value2.x) / 2.f, (value1.y + value2.y) / 2.f,
                            (value1.z + value2.z) / 2.f);
}

Vector3 Vector3::RotationFromAxis(Vector3& axis1, Vector3& axis2, Vector3& axis3)
{
  Vector3 rotation = Vector3::Zero();
  Vector3::RotationFromAxisToRef(axis1, axis2, axis3, rotation);
  return rotation;
}

void Vector3::RotationFromAxisToRef(Vector3& axis1, Vector3& axis2, Vector3& axis3, Vector3& ref)
{
  auto& quat = MathTmp::QuaternionArray[0];
  Quaternion::RotationQuaternionFromAxisToRef(axis1, axis2, axis3, quat);
  quat.toEulerAnglesToRef(ref);
}

} // end of namespace BABYLON
