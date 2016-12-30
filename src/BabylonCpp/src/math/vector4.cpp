#include <babylon/math/vector4.h>

namespace BABYLON {

Vector4::Vector4(float ix, float iy, float iz, float iw)
    : x{ix}, y{iy}, z{iz}, w{iw}
{
}

Vector4::Vector4(const Vector4& otherVector)
    : x{otherVector.x}, y{otherVector.y}, z{otherVector.z}, w{otherVector.w}
{
}

Vector4::Vector4(Vector4&& otherVector)
    : x{std::move(otherVector.x)}
    , y{std::move(otherVector.y)}
    , z{std::move(otherVector.z)}
    , w{std::move(otherVector.w)}
{
}

Vector4& Vector4::operator=(const Vector4& otherVector)
{
  if (&otherVector != this) {
    x = otherVector.x;
    y = otherVector.y;
    z = otherVector.z;
    w = otherVector.w;
  }

  return *this;
}

Vector4& Vector4::operator=(Vector4&& otherVector)
{
  if (&otherVector != this) {
    std::swap(x, otherVector.x);
    std::swap(y, otherVector.y);
    std::swap(z, otherVector.z);
    std::swap(w, otherVector.w);
  }

  return *this;
}

Vector4::~Vector4()
{
}

Vector4 Vector4::copy() const
{
  return Vector4(*this);
}

std::unique_ptr<Vector4> Vector4::clone() const
{
  return std_util::make_unique<Vector4>(*this);
}

std::ostream& operator<<(std::ostream& os, const Vector4& vector)
{
  os << "{\"X\":" << vector.x << ",\"Y\":" << vector.y << ",\"Z\":" << vector.z
     << ",\"W\":" << vector.w << "}";
  return os;
}

const char* Vector4::getClassName() const
{
  return "Vector4";
}

size_t Vector4::getHashCode() const
{
  float tmp = (((x * 0.397f) + y * 3.97f) + z * 39.7f) + w;
  return std_util::to_bitset(tmp).to_ullong();
}

/** Operators **/
Float32Array Vector4::asArray() const
{
  return {x, y, z, w};
}

Vector4& Vector4::toArray(Float32Array& array, unsigned int index)
{
  array[index]     = x;
  array[index + 1] = y;
  array[index + 2] = z;
  array[index + 3] = w;

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
  return Vector4(x + otherVector.x, y + otherVector.y, z + otherVector.z,
                 w + otherVector.w);
}

Vector4& Vector4::addToRef(const Vector4& otherVector, Vector4& result)
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
  return Vector4(x - otherVector.x, y - otherVector.y, z - otherVector.z,
                 w - otherVector.w);
}

Vector4& Vector4::subtractToRef(const Vector4& otherVector, Vector4& result)
{
  result.x = x - otherVector.x;
  result.y = y - otherVector.y;
  result.z = z - otherVector.z;
  result.w = w - otherVector.w;

  return *this;
}

Vector4 Vector4::subtractFromFloats(float ix, float iy, float iz,
                                    float iw) const
{
  return Vector4(x - ix, y - iy, z - iz, w - iw);
}

Vector4& Vector4::subtractFromFloatsToRef(float ix, float iy, float iz,
                                          float iw, Vector4& result)
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

void Vector4::scaleToRef(float iscale, Vector4& result)
{
  result.x = x * iscale;
  result.y = y * iscale;
  result.z = z * iscale;
  result.w = w * iscale;
}

bool Vector4::equals(const Vector4& otherVector) const
{
  return std_util::almost_equal(x, otherVector.x)
         && std_util::almost_equal(y, otherVector.y)
         && std_util::almost_equal(z, otherVector.z)
         && std_util::almost_equal(w, otherVector.w);
}

bool Vector4::equalsWithEpsilon(const Vector4& otherVector, float epsilon) const
{
  return MathTools::WithinEpsilon(x, otherVector.x, epsilon)
         && MathTools::WithinEpsilon(y, otherVector.y, epsilon)
         && MathTools::WithinEpsilon(z, otherVector.z, epsilon)
         && MathTools::WithinEpsilon(w, otherVector.w, epsilon);
}

bool Vector4::equalsToFloats(float ix, float iy, float iz, float iw) const
{
  return std_util::almost_equal(x, ix) && std_util::almost_equal(y, iy)
         && std_util::almost_equal(z, iz) && std_util::almost_equal(w, iw);
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
  return Vector4(x * otherVector.x, y * otherVector.y, z * otherVector.z,
                 w * otherVector.w);
}

Vector4& Vector4::multiplyToRef(const Vector4& otherVector, Vector4& result)
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
  return Vector4(x / otherVector.x, y / otherVector.y, z / otherVector.z,
                 w / otherVector.w);
}

Vector4& Vector4::divideToRef(const Vector4& otherVector, Vector4& result)
{
  result.x = x / otherVector.x;
  result.y = y / otherVector.y;
  result.z = z / otherVector.z;
  result.w = w / otherVector.w;

  return *this;
}

Vector4& Vector4::minimizeInPlace(const Vector4& other)
{
  if (other.x < x)
    x = other.x;
  if (other.y < y)
    y = other.y;
  if (other.z < z)
    z = other.z;
  if (other.w < w)
    w = other.w;

  return *this;
}

Vector4& Vector4::maximizeInPlace(const Vector4& other)
{
  if (other.x > x)
    x = other.x;
  if (other.y > y)
    y = other.y;
  if (other.z > z)
    z = other.z;
  if (other.w > w)
    w = other.w;

  return *this;
}

/** Operator overloading **/
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
  return !equals(otherVector);
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
  float len = length();

  if (std_util::almost_equal(len, 0.f))
    return *this;

  float num = 1.f / len;

  x *= num;
  y *= num;
  z *= num;
  w *= num;

  return *this;
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

/** Statics **/
Vector4 Vector4::FromArray(const Float32Array& array, unsigned int offset)
{
  return Vector4(array[offset], array[offset + 1], array[offset + 2],
                 array[offset + 3]);
}

void Vector4::FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector4& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
  result.w = array[offset + 3];
}

void Vector4::FromFloatsToRef(float ix, float iy, float iz, float iw,
                              Vector4& result)
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
  float x = value1.x - value2.x;
  float y = value1.y - value2.y;
  float z = value1.z - value2.z;
  float w = value1.w - value2.w;

  return (x * x) + (y * y) + (z * z) + (w * w);
}

Vector4 Vector4::Center(const Vector4& value1, const Vector4& value2)
{
  Vector4 center = value1.add(value2);
  center.scaleInPlace(0.5f);
  return center;
}

} // end of namespace BABYLON
