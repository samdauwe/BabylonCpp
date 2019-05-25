#include <babylon/math/vector3.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/axis.h>
#include <babylon/math/math_tmp.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/scalar.h>
#include <babylon/math/vector3.h>
#include <babylon/math/viewport.h>

// SIMD
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
#include <babylon/math/simd/simd_vector3.h>
#endif

namespace BABYLON {

const Vector3 Vector3::_UpReadOnly = Vector3::Up();

Vector3::Vector3(float ix, float iy, float iz) : x{ix}, y{iy}, z{iz}
{
}

Vector3::Vector3(const Vector3& otherVector)
    : x{otherVector.x}, y{otherVector.y}, z{otherVector.z}
{
}

Vector3::Vector3(Vector3&& otherVector)
    : x{std::move(otherVector.x)}
    , y{std::move(otherVector.y)}
    , z{std::move(otherVector.z)}
{
}

Vector3::~Vector3()
{
}

Vector3& Vector3::operator=(const Vector3& otherVector)
{
  if (&otherVector != this) {
    x = otherVector.x;
    y = otherVector.y;
    z = otherVector.z;
  }

  return *this;
}

Vector3& Vector3::operator=(Vector3&& otherVector)
{
  if (&otherVector != this) {
    x = std::move(otherVector.x);
    y = std::move(otherVector.y);
    z = std::move(otherVector.z);
  }

  return *this;
}

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
  return result.copyFromFloats(x + otherVector.x, y + otherVector.y,
                               z + otherVector.z);
}

const Vector3& Vector3::addToRef(const Vector3& otherVector,
                                 Vector3& result) const
{
  return result.copyFromFloats(x + otherVector.x, y + otherVector.y,
                               z + otherVector.z);
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
  return subtractFromFloatsToRef(otherVector.x, otherVector.y, otherVector.z,
                                 result);
}

const Vector3& Vector3::subtractToRef(const Vector3& otherVector,
                                      Vector3& result) const
{
  return subtractFromFloatsToRef(otherVector.x, otherVector.y, otherVector.z,
                                 result);
}

Vector3 Vector3::subtractFromFloats(float ix, float iy, float iz) const
{
  return Vector3(x - ix, y - iy, z - iz);
}

Vector3& Vector3::subtractFromFloatsToRef(float ix, float iy, float iz,
                                          Vector3& result)
{
  return result.copyFromFloats(x - ix, y - iy, z - iz);
}

const Vector3& Vector3::subtractFromFloatsToRef(float ix, float iy, float iz,
                                                Vector3& result) const
{
  return result.copyFromFloats(x - ix, y - iy, z - iz);
}

Vector3 Vector3::negate() const
{
  return Vector3(-x, -y, -z);
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

bool Vector3::equals(const Vector3& otherVector) const
{
  return stl_util::almost_equal(x, otherVector.x)
         && stl_util::almost_equal(y, otherVector.y)
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

const Vector3& Vector3::multiplyToRef(const Vector3& otherVector,
                                      Vector3& result) const
{
  return result.copyFromFloats(x * otherVector.x, y * otherVector.y,
                               z * otherVector.z);
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
  return result.copyFromFloats(x / otherVector.x, y / otherVector.y,
                               z / otherVector.z);
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

  if (!stl_util::almost_equal(absY, absZ)) {
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
  os << "{\"X\":" << vector.x << ",\"Y\":" << vector.y << ",\"Z\":" << vector.z
     << "}";
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

const float& Vector3::operator[](const unsigned int index) const
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
  order = String::toLowerCase(order);
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

Vector3& Vector3::rotateByQuaternionToRef(const Quaternion& quaternion,
                                          Vector3& result)
{
  quaternion.toRotationMatrix(MathTmp::MatrixArray[0]);
  Vector3::TransformCoordinatesToRef(*this, MathTmp::MatrixArray[0], result);
  return result;
}

Vector3& Vector3::rotateByQuaternionAroundPointToRef(
  const Quaternion& quaternion, const Vector3& point, Vector3& result)
{
  subtractToRef(point, MathTmp::Vector3Array[0]);
  MathTmp::Vector3Array[0].rotateByQuaternionToRef(quaternion,
                                                   MathTmp::Vector3Array[0]);
  point.addToRef(MathTmp::Vector3Array[0], result);
  return result;
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
float Vector3::GetClipFactor(const Vector3& vector0, const Vector3& vector1,
                             const Vector3& axis, float size)
{
  const float d0 = Vector3::Dot(vector0, axis) - size;
  const float d1 = Vector3::Dot(vector1, axis) - size;

  const float s = d0 / (d0 - d1);

  return s;
}

float Vector3::GetAngleBetweenVectors(const Vector3& vector0,
                                      const Vector3& vector1,
                                      const Vector3& normal)
{
  const auto v0  = vector0.normalizeToRef(MathTmp::Vector3Array[1]);
  const auto v1  = vector1.normalizeToRef(MathTmp::Vector3Array[2]);
  const auto dot = Vector3::Dot(v0, v1);
  auto& n        = MathTmp::Vector3Array[3];
  Vector3::CrossToRef(v0, v1, n);
  if (Vector3::Dot(n, normal) > 0.f) {
    return std::acos(dot);
  }
  return -std::acos(dot);
}

Vector3 Vector3::FromArray(const Float32Array& array, unsigned int offset)
{
  return Vector3(array[offset], array[offset + 1], array[offset + 2]);
}

void Vector3::FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector3& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
}

void Vector3::FromArrayToRef(const std::array<float, 16>& array,
                             unsigned int offset, Vector3& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
}

void Vector3::FromFloatArrayToRef(const Float32Array& array,
                                  unsigned int offset, Vector3& result)
{
  return Vector3::FromArrayToRef(array, offset, result);
}

void Vector3::FromFloatArrayToRef(const std::array<float, 16>& array,
                                  unsigned int offset, Vector3& result)
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

Vector3 Vector3::Down()
{
  return Vector3(0.f, -1.f, 0.f);
}

Vector3 Vector3::Forward()
{
  return Vector3(0.f, 0.f, 1.f);
}

Vector3 Vector3::Backward()
{
  return Vector3(0.f, 0.f, -1.f);
}

Vector3 Vector3::Right()
{
  return Vector3(1.f, 0.f, 0.f);
}

Vector3 Vector3::Left()
{
  return Vector3(-1.f, 0.f, 0.f);
}

Vector3 Vector3::TransformCoordinates(const Vector3& vector,
                                      const Matrix& transformation)
{
  Vector3 result = Vector3::Zero();
  Vector3::TransformCoordinatesToRef(vector, transformation, result);
  return result;
}

void Vector3::TransformCoordinatesToRef(const Vector3& vector,
                                        const Matrix& transformation,
                                        Vector3& result)
{
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
  SIMD::SIMDVector3::TransformCoordinatesToRefSIMD(vector, transformation,
                                                   result);
#else
  Vector3::TransformCoordinatesFromFloatsToRef(vector.x, vector.y, vector.z,
                                               transformation, result);
#endif
}

void Vector3::TransformCoordinatesFromFloatsToRef(float x, float y, float z,
                                                  const Matrix& transformation,
                                                  Vector3& result)
{
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
  SIMD::SIMDVector3::TransformCoordinatesFromFloatsToRefSIMD(
    x, y, z, transformation, result);
#else
  const auto& m = transformation.m();
  const auto rx = x * m[0] + y * m[4] + z * m[8] + m[12];
  const auto ry = x * m[1] + y * m[5] + z * m[9] + m[13];
  const auto rz = x * m[2] + y * m[6] + z * m[10] + m[14];
  const auto rw = 1 / (x * m[3] + y * m[7] + z * m[11] + m[15]);

  result.x = rx * rw;
  result.y = ry * rw;
  result.z = rz * rw;
#endif
}

Vector3 Vector3::TransformNormal(const Vector3& vector,
                                 const Matrix& transformation)
{
  Vector3 result = Vector3::Zero();
  Vector3::TransformNormalToRef(vector, transformation, result);
  return result;
}

void Vector3::TransformNormalToRef(const Vector3& vector,
                                   const Matrix& transformation,
                                   Vector3& result)
{
  TransformNormalFromFloatsToRef(vector.x, vector.y, vector.z, transformation,
                                 result);
}

void Vector3::TransformNormalFromFloatsToRef(float x, float y, float z,
                                             const Matrix& transformation,
                                             Vector3& result)
{
  const auto& m = transformation.m();
  result.x      = x * m[0] + y * m[4] + z * m[8];
  result.y      = x * m[1] + y * m[5] + z * m[9];
  result.z      = x * m[2] + y * m[6] + z * m[10];
}

Vector3 Vector3::CatmullRom(const Vector3& value1, const Vector3& value2,
                            const Vector3& value3, const Vector3& value4,
                            float amount)
{
  const float squared = amount * amount;
  const float cubed   = amount * squared;

  const float x
    = 0.5f
      * ((((2.f * value2.x) + ((-value1.x + value3.x) * amount))
          + (((((2.f * value1.x) - (5.f * value2.x)) + (4.f * value3.x))
              - value4.x)
             * squared))
         + ((((-value1.x + (3.f * value2.x)) - (3.f * value3.x)) + value4.x)
            * cubed));

  const float y
    = 0.5f
      * ((((2.f * value2.y) + ((-value1.y + value3.y) * amount))
          + (((((2.f * value1.y) - (5.f * value2.y)) + (4.f * value3.y))
              - value4.y)
             * squared))
         + ((((-value1.y + (3.f * value2.y)) - (3.f * value3.y)) + value4.y)
            * cubed));

  const float z
    = 0.5f
      * ((((2.f * value2.z) + ((-value1.z + value3.z) * amount))
          + (((((2.f * value1.z) - (5.f * value2.z)) + (4.f * value3.z))
              - value4.z)
             * squared))
         + ((((-value1.z + (3.f * value2.z)) - (3.f * value3.z)) + value4.z)
            * cubed));

  return Vector3(x, y, z);
}

Vector3 Vector3::Clamp(const Vector3& value, const Vector3& min,
                       const Vector3& max)
{
  Vector3 v;
  Vector3::ClampToRef(value, min, max, v);
  return v;
}

void Vector3::ClampToRef(const Vector3& value, const Vector3& min,
                         const Vector3& max, Vector3& result)
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

Vector3 Vector3::Hermite(const Vector3& value1, const Vector3& tangent1,
                         const Vector3& value2, const Vector3& tangent2,
                         float amount)
{
  const float squared = amount * amount;
  const float cubed   = amount * squared;
  const float part1   = ((2.f * cubed) - (3.f * squared)) + 1.f;
  const float part2   = (-2.f * cubed) + (3.f * squared);
  const float part3   = (cubed - (2.f * squared)) + amount;
  const float part4   = cubed - squared;

  const float x
    = (((value1.x * part1) + (value2.x * part2)) + (tangent1.x * part3))
      + (tangent2.x * part4);
  const float y
    = (((value1.y * part1) + (value2.y * part2)) + (tangent1.y * part3))
      + (tangent2.y * part4);
  const float z
    = (((value1.z * part1) + (value2.z * part2)) + (tangent1.z * part3))
      + (tangent2.z * part4);

  return Vector3(x, y, z);
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float amount)
{
  Vector3 result(0.f, 0.f, 0.f);
  Vector3::LerpToRef(start, end, amount, result);
  return result;
}

void Vector3::LerpToRef(const Vector3& start, const Vector3& end, float amount,
                        Vector3& result)
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

void Vector3::CrossToRef(const Vector3& left, const Vector3& right,
                         Vector3& result)
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

Vector3 Vector3::Project(const Vector3& vector, Matrix& world,
                         Matrix& transform, const Viewport& viewport)
{
  const auto cw = static_cast<float>(viewport.width);
  const auto ch = static_cast<float>(viewport.height);
  const auto cx = static_cast<float>(viewport.x);
  const auto cy = static_cast<float>(viewport.y);

  auto& viewportMatrix = MathTmp::MatrixArray[1];

  Matrix::FromValuesToRef(cw / 2.f, 0.f, 0.f, 0.f,  //
                          0.f, -ch / 2.f, 0.f, 0.f, //
                          0.f, 0.f, 0.5f, 0.f,      //
                          cx + cw / 2.f, ch / 2.f + cy, 0.5f, 1.f,
                          viewportMatrix);

  auto& matrix = MathTmp::MatrixArray[0];
  world.multiplyToRef(transform, matrix);
  matrix.multiplyToRef(viewportMatrix, matrix);

  return Vector3::TransformCoordinates(vector, matrix);
}

void Vector3::_UnprojectFromInvertedMatrixToRef(const Vector3& source,
                                                const Matrix& matrix,
                                                Vector3& result)
{
  Vector3::TransformCoordinatesToRef(source, matrix, result);
  const auto& m  = matrix.m();
  const auto num = source.x * m[3] + source.y * m[7] + source.z * m[11] + m[15];
  if (Scalar::WithinEpsilon(num, 1.f)) {
    result.scaleInPlace(1.f / num);
  }
}

Vector3 Vector3::UnprojectFromTransform(Vector3& source, float viewportWidth,
                                        float viewportHeight, Matrix& world,
                                        Matrix& transform)
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

Vector3 Vector3::Unproject(const Vector3& source, float viewportWidth,
                           float viewportHeight, Matrix& world, Matrix& view,
                           Matrix& projection)
{
  auto result = Vector3::Zero();

  Vector3::UnprojectToRef(source, viewportWidth, viewportHeight, world, view,
                          projection, result);

  return result;
}

void Vector3::UnprojectToRef(const Vector3& source, float viewportWidth,
                             float viewportHeight, Matrix& world, Matrix& view,
                             Matrix& projection, Vector3& result)
{
  Vector3::UnprojectFloatsToRef(source.x, source.y, source.z, viewportWidth,
                                viewportHeight, world, view, projection,
                                result);
}

void Vector3::UnprojectFloatsToRef(float sourceX, float sourceY, float sourceZ,
                                   float viewportWidth, float viewportHeight,
                                   Matrix& world, Matrix& view,
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

Vector3 Vector3::Center(const Vector3& value1, const Vector3& value2)
{
  Vector3 center = value1.add(value2);
  center.scaleInPlace(0.5f);
  return center;
}

Vector3 Vector3::RotationFromAxis(Vector3& axis1, Vector3& axis2,
                                  Vector3& axis3)
{
  Vector3 rotation = Vector3::Zero();
  Vector3::RotationFromAxisToRef(axis1, axis2, axis3, rotation);
  return rotation;
}

void Vector3::RotationFromAxisToRef(Vector3& axis1, Vector3& axis2,
                                    Vector3& axis3, Vector3& ref)
{
  auto& quat = MathTmp::QuaternionArray[0];
  Quaternion::RotationQuaternionFromAxisToRef(axis1, axis2, axis3, quat);
  quat.toEulerAnglesToRef(ref);
}

} // end of namespace BABYLON
