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

Vector3::Vector3(float ix, float iy, float iz) : x{ix}, y{iy}, z{iz}
{
}

Vector3::Vector3(const Vector3& otherVector)
    : x{otherVector.x}, y{otherVector.y}, z{otherVector.z}
{
}

Vector3::Vector3(Vector3&& otherVector)
    : x{::std::move(otherVector.x)}
    , y{::std::move(otherVector.y)}
    , z{::std::move(otherVector.z)}
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
    x = ::std::move(otherVector.x);
    y = ::std::move(otherVector.y);
    z = ::std::move(otherVector.z);
  }

  return *this;
}

Vector3 Vector3::copy() const
{
  return Vector3(*this);
}

unique_ptr_t<Vector3> Vector3::clone() const
{
  return ::std::make_unique<Vector3>(*this);
}

string_t Vector3::toString() const
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
  return Quaternion::RotationYawPitchRoll(x, y, z);
}

Vector3& Vector3::addInPlace(const Vector3& otherVector)
{
  x += otherVector.x;
  y += otherVector.y;
  z += otherVector.z;

  return *this;
}

Vector3 Vector3::add(const Vector3& otherVector) const
{
  return Vector3(x + otherVector.x, y + otherVector.y, z + otherVector.z);
}

const Vector3& Vector3::addToRef(const Vector3& otherVector,
                                 Vector3& result) const
{
  result.x = x + otherVector.x;
  result.y = y + otherVector.y;
  result.z = z + otherVector.z;

  return *this;
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

const Vector3& Vector3::subtractToRef(const Vector3& otherVector,
                                      Vector3& result) const
{
  result.x = x - otherVector.x;
  result.y = y - otherVector.y;
  result.z = z - otherVector.z;

  return *this;
}

Vector3 Vector3::subtractFromFloats(float ix, float iy, float iz) const
{
  return Vector3(x - ix, y - iy, z - iz);
}

const Vector3& Vector3::subtractFromFloatsToRef(float ix, float iy, float iz,
                                                Vector3& result) const
{
  result.x = x - ix;
  result.y = y - iy;
  result.z = z - iz;

  return *this;
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
  result.x = x * static_cast<float>(iscale);
  result.y = y * static_cast<float>(iscale);
  result.z = z * static_cast<float>(iscale);

  return *this;
}

const Vector3& Vector3::scaleToRef(float iscale, Vector3& result) const
{
  result.x = x * iscale;
  result.y = y * iscale;
  result.z = z * iscale;

  return *this;
}

const Vector3& Vector3::scaleAndAddToRef(int iscale, Vector3& result) const
{
  result.x += x * static_cast<float>(iscale);
  result.y += y * static_cast<float>(iscale);
  result.z += z * static_cast<float>(iscale);

  return *this;
}

const Vector3& Vector3::scaleAndAddToRef(float iscale, Vector3& result) const
{
  result.x += x * iscale;
  result.y += y * iscale;
  result.z += z * iscale;

  return *this;
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
  return Vector3(x * otherVector.x, y * otherVector.y, z * otherVector.z);
}

const Vector3& Vector3::multiplyToRef(const Vector3& otherVector,
                                      Vector3& result) const
{
  result.x = x * otherVector.x;
  result.y = y * otherVector.y;
  result.z = z * otherVector.z;

  return *this;
}

Vector3 Vector3::multiplyByFloats(float ix, float iy, float iz) const
{
  return Vector3(x * ix, y * iy, z * iz);
}

Vector3 Vector3::divide(const Vector3& otherVector) const
{
  return Vector3(x / otherVector.x, y / otherVector.y, z / otherVector.z);
}

const Vector3& Vector3::divideToRef(const Vector3& otherVector,
                                    Vector3& result) const
{
  result.x = x / otherVector.x;
  result.y = y / otherVector.y;
  result.z = z / otherVector.z;

  return *this;
}

Vector3& Vector3::divideInPlace(const Vector3& otherVector)
{
  x /= otherVector.x;
  y /= otherVector.y;
  z /= otherVector.z;

  return *this;
}

Vector3& Vector3::minimizeInPlace(const Vector3& other)
{
  if (other.x < x) {
    x = other.x;
  }
  if (other.y < y) {
    y = other.y;
  }
  if (other.z < z) {
    z = other.z;
  }

  return *this;
}

Vector3& Vector3::maximizeInPlace(const Vector3& other)
{
  if (other.x > x) {
    x = other.x;
  }
  if (other.y > y) {
    y = other.y;
  }
  if (other.z > z) {
    z = other.z;
  }

  return *this;
}

bool Vector3::isNonUniform() const
{
  const auto absX = ::std::abs(x);
  const auto absY = ::std::abs(y);
  if (!stl_util::almost_equal(absX, absY)) {
    return true;
  }

  const auto absZ = ::std::abs(z);
  if (!stl_util::almost_equal(absX, absZ)) {
    return true;
  }

  if (!stl_util::almost_equal(absY, absZ)) {
    return true;
  }

  return false;
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
  return ::std::sqrt(x * x + y * y + z * z);
}

float Vector3::lengthSquared() const
{
  return (x * x + y * y + z * z);
}

/** Methods **/
Vector3& Vector3::normalize()
{
  const float len = length();
  if (stl_util::almost_equal(len, 0.f) || stl_util::almost_equal(len, 1.f)) {
    return *this;
  }

  const float num = 1.f / len;

  x *= num;
  y *= num;
  z *= num;

  return *this;
}

Vector3 Vector3::normalizeToNew() const
{
  Vector3 normalized(0.f, 0.f, 0.f);
  normalizeToRef(normalized);
  return normalized;
}

Vector3 Vector3::normalizeToRef(Vector3& reference) const
{
  const float len = length();
  if (stl_util::almost_equal(len, 0.f) || stl_util::almost_equal(len, 1.f)) {
    reference.set(x, y, z);
    return reference;
  }

  const auto scale = 1.f / len;
  scaleToRef(scale, reference);
  return reference;
}

Vector3& Vector3::copyFrom(const Vector3& source)
{
  x = source.x;
  y = source.y;
  z = source.z;

  return *this;
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
  const auto v0  = vector0.copy().normalize();
  const auto v1  = vector1.copy().normalize();
  const auto dot = Vector3::Dot(v0, v1);
  const auto n   = Vector3::Cross(v0, v1);
  if (Vector3::Dot(n, normal) > 0.f) {
    return ::std::acos(dot);
  }
  return -::std::acos(dot);
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

void Vector3::FromArrayToRef(const array_t<float, 16>& array,
                             unsigned int offset, Vector3& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
}

void Vector3::FromFloatsToRef(float ix, float iy, float iz, Vector3& result)
{
  result.x = ix;
  result.y = iy;
  result.z = iz;
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

Vector3 Vector3::Down()
{
  return Vector3(0.f, -1.f, 0.f);
}

Vector3 Vector3::Forward()
{
  return Vector3(0.f, 0.f, 1.f);
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
  const float x = (vector.x * transformation.m[0])
                  + (vector.y * transformation.m[4])
                  + (vector.z * transformation.m[8]) + transformation.m[12];
  const float y = (vector.x * transformation.m[1])
                  + (vector.y * transformation.m[5])
                  + (vector.z * transformation.m[9]) + transformation.m[13];
  const float z = (vector.x * transformation.m[2])
                  + (vector.y * transformation.m[6])
                  + (vector.z * transformation.m[10]) + transformation.m[14];
  const float w = (vector.x * transformation.m[3])
                  + (vector.y * transformation.m[7])
                  + (vector.z * transformation.m[11]) + transformation.m[15];

  result.x       = x / w;
  result.y       = y / w;
  result.z       = z / w;
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
  const float rx = (x * transformation.m[0]) + (y * transformation.m[4])
                   + (z * transformation.m[8]) + transformation.m[12];
  const float ry = (x * transformation.m[1]) + (y * transformation.m[5])
                   + (z * transformation.m[9]) + transformation.m[13];
  const float rz = (x * transformation.m[2]) + (y * transformation.m[6])
                   + (z * transformation.m[10]) + transformation.m[14];
  const float rw = (x * transformation.m[3]) + (y * transformation.m[7])
                   + (z * transformation.m[11]) + transformation.m[15];

  result.x = rx / rw;
  result.y = ry / rw;
  result.z = rz / rw;
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
  result.x = (vector.x * transformation.m[0]) + (vector.y * transformation.m[4])
             + (vector.z * transformation.m[8]);
  result.y = (vector.x * transformation.m[1]) + (vector.y * transformation.m[5])
             + (vector.z * transformation.m[9]);
  result.z = (vector.x * transformation.m[2]) + (vector.y * transformation.m[6])
             + (vector.z * transformation.m[10]);
}

void Vector3::TransformNormalFromFloatsToRef(float x, float y, float z,
                                             const Matrix& transformation,
                                             Vector3& result)
{
  result.x = (x * transformation.m[0]) + (y * transformation.m[4])
             + (z * transformation.m[8]);
  result.y = (x * transformation.m[1]) + (y * transformation.m[5])
             + (z * transformation.m[9]);
  result.z = (x * transformation.m[2]) + (y * transformation.m[6])
             + (z * transformation.m[10]);
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
  float x = value.x;

  x = (x > max.x) ? max.x : x;
  x = (x < min.x) ? min.x : x;

  float y = value.y;

  y = (y > max.y) ? max.y : y;
  y = (y < min.y) ? min.y : y;

  float z = value.z;

  z = (z > max.z) ? max.z : z;
  z = (z < min.z) ? min.z : z;

  return Vector3(x, y, z);
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
  Vector3 result = Vector3::Zero();

  Vector3::CrossToRef(left, right, result);

  return result;
}

void Vector3::CrossToRef(const Vector3& left, const Vector3& right,
                         Vector3& result)
{
  MathTmp::Vector3Array[0].x = left.y * right.z - left.z * right.y;
  MathTmp::Vector3Array[0].y = left.z * right.x - left.x * right.z;
  MathTmp::Vector3Array[0].z = left.x * right.y - left.y * right.x;
  result.copyFrom(MathTmp::Vector3Array[0]);
}

Vector3 Vector3::Normalize(const Vector3& vector)
{
  Vector3 result = Vector3::Zero();
  Vector3::NormalizeToRef(vector, result);
  return result;
}

void Vector3::NormalizeToRef(const Vector3& vector, Vector3& result)
{
  result.copyFrom(vector);
  result.normalize();
}

Vector3 Vector3::Project(const Vector3& vector, Matrix& world,
                         Matrix& transform, const Viewport& viewport)
{
  const float cw = static_cast<float>(viewport.width);
  const float ch = static_cast<float>(viewport.height);
  const float cx = static_cast<float>(viewport.x);
  const float cy = static_cast<float>(viewport.y);

  static Matrix viewportMatrix;

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

Vector3 Vector3::UnprojectFromTransform(Vector3& source, float viewportWidth,
                                        float viewportHeight, Matrix& world,
                                        Matrix& transform)
{
  auto& matrix = MathTmp::MatrixArray[0];
  world.multiplyToRef(transform, matrix);
  matrix.invert();
  source.x       = source.x / viewportWidth * 2.f - 1.f;
  source.y       = -(source.y / viewportHeight * 2.f - 1.f);
  auto vector    = Vector3::TransformCoordinates(source, matrix);
  const auto num = source.x * matrix.m[3] + source.y * matrix.m[7]
                   + source.z * matrix.m[11] + matrix.m[15];

  if (Scalar::WithinEpsilon(num, 1.f)) {
    vector = vector.scale(1.f / num);
  }

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
  Vector3::TransformCoordinatesToRef(screenSource, matrix, result);
  auto num = screenSource.x * matrix.m[3] + screenSource.y * matrix.m[7]
             + screenSource.z * matrix.m[11] + matrix.m[15];

  if (Scalar::WithinEpsilon(num, 1.f)) {
    result.scaleInPlace(1.f / num);
  }
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
  return ::std::sqrt(Vector3::DistanceSquared(value1, value2));
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
