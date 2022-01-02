#include <babylon/maths/vector4.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>

namespace BABYLON {

Vector4::Vector4(float ix, float iy, float iz, float iw) : x{ix}, y{iy}, z{iz}, w{iw}
{
}

Vector4::Vector4(const Vector4& otherVector) = default;

Vector4::Vector4(Vector4&& otherVector) = default;

Vector4& Vector4::operator=(const Vector4& otherVector) = default;

Vector4& Vector4::operator=(Vector4&& otherVector) = default;

Vector4::~Vector4() = default;

Vector4 Vector4::copy() const
{
  return Vector4(*this);
}

std::unique_ptr<Vector4> Vector4::clone() const
{
  return std::make_unique<Vector4>(*this);
}

std::string Vector4::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

const char* Vector4::getClassName() const
{
  return "Vector4";
}

size_t Vector4::getHashCode() const
{
  const float tmp = (((x * 0.397f) + y * 3.97f) + z * 39.7f) + w;
  return stl_util::to_bitset(tmp).to_ullong();
}

/** Operators **/
Float32Array Vector4::asArray() const
{
  Float32Array result;
  toArray(result, 0);

  return result;
}

const Vector4& Vector4::toArray(Float32Array& array, unsigned int index) const
{
  array[index]     = x;
  array[index + 1] = y;
  array[index + 2] = z;
  array[index + 3] = w;

  return *this;
}

Vector4& Vector4::fromArray(const Float32Array& array, unsigned int index)
{
  Vector4::FromArrayToRef(array, index, *this);
  return *this;
}

Vector4& Vector4::addInPlace(const Vector4& otherVector)
{
  x += otherVector.x;
  y += otherVector.y;
  z += otherVector.z;
  w += otherVector.w;

  return *this;
}

Vector4 Vector4::add(const Vector4& otherVector) const
{
  return Vector4(x + otherVector.x, y + otherVector.y, z + otherVector.z, w + otherVector.w);
}

const Vector4& Vector4::addToRef(const Vector4& otherVector, Vector4& result) const
{
  result.x = x + otherVector.x;
  result.y = y + otherVector.y;
  result.z = z + otherVector.z;
  result.w = w + otherVector.w;

  return *this;
}

Vector4& Vector4::subtractInPlace(const Vector4& otherVector)
{
  x -= otherVector.x;
  y -= otherVector.y;
  z -= otherVector.z;
  w -= otherVector.w;

  return *this;
}

Vector4 Vector4::subtract(const Vector4& otherVector) const
{
  return Vector4(x - otherVector.x, y - otherVector.y, z - otherVector.z, w - otherVector.w);
}

const Vector4& Vector4::subtractToRef(const Vector4& otherVector, Vector4& result) const
{
  result.x = x - otherVector.x;
  result.y = y - otherVector.y;
  result.z = z - otherVector.z;
  result.w = w - otherVector.w;

  return *this;
}

Vector4 Vector4::subtractFromFloats(float ix, float iy, float iz, float iw) const
{
  return Vector4(x - ix, y - iy, z - iz, w - iw);
}

const Vector4& Vector4::subtractFromFloatsToRef(float ix, float iy, float iz, float iw,
                                                Vector4& result) const
{
  result.x = x - ix;
  result.y = y - iy;
  result.z = z - iz;
  result.w = w - iw;

  return *this;
}

Vector4 Vector4::negate() const
{
  return Vector4(-x, -y, -z, -w);
}

Vector4& Vector4::negateInPlace()
{
  x *= -1.f;
  y *= -1.f;
  z *= -1.f;
  w *= -1.f;
  return *this;
}

Vector4 Vector4::negateToRef(Vector4& result)
{
  return result.copyFromFloats(x * -1.f, y * -1.f, z * -1.f, w * -1.f);
}

Vector4& Vector4::scaleInPlace(float iscale)
{
  x *= iscale;
  y *= iscale;
  z *= iscale;
  w *= iscale;

  return *this;
}

Vector4 Vector4::scale(float iscale) const
{
  return Vector4(x * iscale, y * iscale, z * iscale, w * iscale);
}

const Vector4& Vector4::scaleToRef(float iscale, Vector4& result) const
{
  result.x = x * iscale;
  result.y = y * iscale;
  result.z = z * iscale;
  result.w = w * iscale;

  return *this;
}

const Vector4& Vector4::scaleAndAddToRef(float iscale, Vector4& result) const
{
  result.x += x * iscale;
  result.y += y * iscale;
  result.z += z * iscale;
  result.w += w * iscale;

  return *this;
}

bool Vector4::equals(const Vector4& otherVector) const
{
  return stl_util::almost_equal(x, otherVector.x) && stl_util::almost_equal(y, otherVector.y)
         && stl_util::almost_equal(z, otherVector.z) && stl_util::almost_equal(w, otherVector.w);
}

bool Vector4::equalsWithEpsilon(const Vector4& otherVector, float epsilon) const
{
  return Scalar::WithinEpsilon(x, otherVector.x, epsilon)
         && Scalar::WithinEpsilon(y, otherVector.y, epsilon)
         && Scalar::WithinEpsilon(z, otherVector.z, epsilon)
         && Scalar::WithinEpsilon(w, otherVector.w, epsilon);
}

bool Vector4::equalsToFloats(float ix, float iy, float iz, float iw) const
{
  return stl_util::almost_equal(x, ix) && stl_util::almost_equal(y, iy)
         && stl_util::almost_equal(z, iz) && stl_util::almost_equal(w, iw);
}

Vector4& Vector4::multiplyInPlace(const Vector4& otherVector)
{
  x *= otherVector.x;
  y *= otherVector.y;
  z *= otherVector.z;
  w *= otherVector.w;

  return *this;
}

Vector4 Vector4::multiply(const Vector4& otherVector) const
{
  return Vector4(x * otherVector.x, y * otherVector.y, z * otherVector.z, w * otherVector.w);
}

const Vector4& Vector4::multiplyToRef(const Vector4& otherVector, Vector4& result) const
{
  result.x = x * otherVector.x;
  result.y = y * otherVector.y;
  result.z = z * otherVector.z;
  result.w = w * otherVector.w;

  return *this;
}

Vector4 Vector4::multiplyByFloats(float ix, float iy, float iz, float iw) const
{
  return Vector4(x * ix, y * iy, z * iz, w * iw);
}

Vector4 Vector4::divide(const Vector4& otherVector) const
{
  return Vector4(x / otherVector.x, y / otherVector.y, z / otherVector.z, w / otherVector.w);
}

const Vector4& Vector4::divideToRef(const Vector4& otherVector, Vector4& result) const
{
  result.x = x / otherVector.x;
  result.y = y / otherVector.y;
  result.z = z / otherVector.z;
  result.w = w / otherVector.w;

  return *this;
}

Vector4& Vector4::divideInPlace(const Vector4& otherVector)
{
  x /= otherVector.x;
  y /= otherVector.y;
  z /= otherVector.z;
  w /= otherVector.w;

  return *this;
}

Vector4& Vector4::minimizeInPlace(const Vector4& other)
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
  if (other.w < w) {
    w = other.w;
  }

  return *this;
}

Vector4& Vector4::maximizeInPlace(const Vector4& other)
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
  if (other.w > w) {
    w = other.w;
  }

  return *this;
}

Vector4 Vector4::floor() const
{
  return Vector4(std::floor(x), //
                 std::floor(y), //
                 std::floor(z), //
                 std::floor(w)  //
  );
}

/**
 * @brief Gets a new Vector4 from current Vector3 floored values.
 * @returns a new Vector4
 */
Vector4 Vector4::fract() const
{
  return Vector4(x - std::floor(x), //
                 y - std::floor(y), //
                 z - std::floor(z), //
                 w - std::floor(w)  //
  );
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Vector4& vector)
{
  os << "{\"X\":" << vector.x << ",\"Y\":" << vector.y << ",\"Z\":" << vector.z
     << ",\"W\":" << vector.w << "}";
  return os;
}

Vector4 Vector4::operator+(const Vector4& otherVector) const
{
  return add(otherVector);
}

Vector4& Vector4::operator+=(const Vector4& otherVector)
{
  return addInPlace(otherVector);
}

Vector4 Vector4::operator-(const Vector4& otherVector) const
{
  return subtract(otherVector);
}

Vector4& Vector4::operator-=(const Vector4& otherVector)
{
  return subtractInPlace(otherVector);
}

Vector4 Vector4::operator-() const
{
  return negate();
}

Vector4 Vector4::operator*(float scaleVal) const
{
  return scale(scaleVal);
}

Vector4& Vector4::operator*=(float scaleVal)
{
  return scaleInPlace(scaleVal);
}

Vector4 Vector4::operator*(const Vector4& otherVector) const
{
  return multiply(otherVector);
}

Vector4& Vector4::operator*=(const Vector4& otherVector)
{
  return multiplyInPlace(otherVector);
}

Vector4 Vector4::operator/(const Vector4& otherVector) const
{
  return divide(otherVector);
}

bool Vector4::operator==(const Vector4& otherVector) const
{
  return equals(otherVector);
}

bool Vector4::operator!=(const Vector4& otherVector) const
{
  return !(operator==(otherVector));
}

/** Properties **/
float Vector4::length() const
{
  return std::sqrt(x * x + y * y + z * z + w * w);
}

float Vector4::lengthSquared() const
{
  return (x * x + y * y + z * z + w * w);
}

/** Methods **/
Vector4& Vector4::normalize()
{
  const auto len = length();

  if (stl_util::almost_equal(len, 0.f)) {
    return *this;
  }

  return scaleInPlace(1.f / len);
}

Vector3 Vector4::toVector3() const
{
  return Vector3(x, y, z);
}

Vector4& Vector4::copyFrom(const Vector4& source)
{
  x = source.x;
  y = source.y;
  z = source.z;
  w = source.w;

  return *this;
}

Vector4& Vector4::copyFromFloats(float ix, float iy, float iz, float iw)
{
  x = ix;
  y = iy;
  z = iz;
  w = iw;

  return *this;
}

Vector4& Vector4::set(float ix, float iy, float iz, float iw)
{
  return copyFromFloats(ix, iy, iz, iw);
}

Vector4& Vector4::setAll(float v)
{
  x = y = z = w = v;
  return *this;
}

/** Statics **/
Vector4 Vector4::FromArray(const Float32Array& array, unsigned int offset)
{
  return Vector4(array[offset], array[offset + 1], array[offset + 2], array[offset + 3]);
}

void Vector4::FromArrayToRef(const Float32Array& array, unsigned int offset, Vector4& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
  result.w = array[offset + 3];
}

void Vector4::FromFloatsToRef(float ix, float iy, float iz, float iw, Vector4& result)
{
  result.x = ix;
  result.y = iy;
  result.z = iz;
  result.w = iw;
}

Vector4 Vector4::Zero()
{
  return Vector4(0.f, 0.f, 0.f, 0.f);
}

Vector4 Vector4::One()
{
  return Vector4(1.f, 1.f, 1.f, 1.f);
}

Vector4 Vector4::Normalize(const Vector4& vector)
{
  Vector4 result = Vector4::Zero();
  Vector4::NormalizeToRef(vector, result);
  return result;
}

void Vector4::NormalizeToRef(const Vector4& vector, Vector4& result)
{
  result.copyFrom(vector);
  result.normalize();
}

Vector4 Vector4::Minimize(const Vector4& left, const Vector4& right)
{
  Vector4 min = left;
  min.minimizeInPlace(right);
  return min;
}

Vector4 Vector4::Maximize(const Vector4& left, const Vector4& right)
{
  Vector4 max = left;
  max.maximizeInPlace(right);
  return max;
}

float Vector4::Distance(const Vector4& value1, const Vector4& value2)
{
  return std::sqrt(Vector4::DistanceSquared(value1, value2));
}

float Vector4::DistanceSquared(const Vector4& value1, const Vector4& value2)
{
  const auto x = value1.x - value2.x;
  const auto y = value1.y - value2.y;
  const auto z = value1.z - value2.z;
  const auto w = value1.w - value2.w;

  return (x * x) + (y * y) + (z * z) + (w * w);
}

Vector4 Vector4::Center(const Vector4& value1, const Vector4& value2)
{
  auto center = Vector4::Zero();
  Vector4::CenterToRef(value1, value2, center);
  return center;
}

Vector4& Vector4::CenterToRef(const Vector4& value1, const Vector4& value2, Vector4& ref)
{
  return ref.copyFromFloats((value1.x + value2.x) / 2.f, (value1.y + value2.y) / 2.f,
                            (value1.z + value2.z) / 2.f, (value1.w + value2.w) / 2.f);
}

Vector4 Vector4::TransformNormal(const Vector4& vector, const Matrix& transformation)
{
  auto result = Vector4::Zero();
  Vector4::TransformNormalToRef(vector, transformation, result);
  return result;
}

void Vector4::TransformNormalToRef(const Vector4& vector, const Matrix& transformation,
                                   Vector4& result)
{
  const auto& m = transformation.m();
  const auto x  = (vector.x * m[0]) + (vector.y * m[4]) + (vector.z * m[8]);
  const auto y  = (vector.x * m[1]) + (vector.y * m[5]) + (vector.z * m[9]);
  const auto z  = (vector.x * m[2]) + (vector.y * m[6]) + (vector.z * m[10]);
  result.x      = x;
  result.y      = y;
  result.z      = z;
  result.w      = vector.w;
}

void Vector4::TransformNormalFromFloatsToRef(float x, float y, float z, float w,
                                             const Matrix& transformation, Vector4& result)
{
  const auto& m = transformation.m();
  result.x      = (x * m[0]) + (y * m[4]) + (z * m[8]);
  result.y      = (x * m[1]) + (y * m[5]) + (z * m[9]);
  result.z      = (x * m[2]) + (y * m[6]) + (z * m[10]);
  result.w      = w;
}

Vector4 Vector4::FromVector3(const Vector3& source, float w)
{
  return Vector4(source.x, source.y, source.z, w);
}

} // end of namespace BABYLON
