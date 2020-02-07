#include <babylon/maths/vector2.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

Vector2::Vector2(float ix, float iy) : x{ix}, y{iy}
{
}

Vector2::Vector2(int ix, int iy) : x{static_cast<float>(ix)}, y{static_cast<float>(iy)}
{
}

Vector2::Vector2(const Vector2& otherVector) = default;

Vector2::Vector2(Vector2&& otherVector) = default;

Vector2& Vector2::operator=(const Vector2& otherVector) = default;

Vector2& Vector2::operator=(Vector2&& otherVector) = default;

Vector2::~Vector2() = default;

Vector2 Vector2::copy() const
{
  return Vector2(*this);
}

std::unique_ptr<Vector2> Vector2::clone() const
{
  return std::make_unique<Vector2>(*this);
}

std::string Vector2::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

const char* Vector2::getClassName() const
{
  return "Vector2";
}

size_t Vector2::getHashCode() const
{
  const float tmp = x * 0.397f + y;
  return stl_util::to_bitset(tmp).to_ullong();
}

/** Operators **/
const Vector2& Vector2::toArray(Float32Array& array, unsigned int index) const
{
  array[index]     = x;
  array[index + 1] = y;

  return *this;
}

Float32Array Vector2::asArray() const
{
  Float32Array result;
  toArray(result, 0);

  return result;
}

Vector2& Vector2::copyFrom(const Vector2& source)
{
  x = source.x;
  y = source.y;

  return *this;
}

Vector2& Vector2::copyFromFloats(float ix, float iy)
{
  x = ix;
  y = iy;

  return *this;
}

Vector2& Vector2::set(float ix, float iy)
{
  return copyFromFloats(ix, iy);
}

Vector2 Vector2::add(const Vector2& otherVector) const
{
  return Vector2(x + otherVector.x, y + otherVector.y);
}

const Vector2& Vector2::addToRef(const Vector2& otherVector, Vector2& result) const
{
  result.x = x + otherVector.x;
  result.y = y + otherVector.y;

  return *this;
}

Vector2& Vector2::addInPlace(const Vector2& otherVector)
{
  x += otherVector.x;
  y += otherVector.y;

  return *this;
}

Vector2 Vector2::addVector3(const Vector3& otherVector) const
{
  return Vector2(x + otherVector.x, y + otherVector.y);
}

Vector2 Vector2::subtract(const Vector2& otherVector) const
{
  return Vector2(x - otherVector.x, y - otherVector.y);
}

const Vector2& Vector2::subtractToRef(const Vector2& otherVector, Vector2& result) const
{
  result.x = x - otherVector.x;
  result.y = y - otherVector.y;

  return *this;
}

Vector2& Vector2::subtractInPlace(const Vector2& otherVector)
{
  x -= otherVector.x;
  y -= otherVector.y;

  return *this;
}

Vector2& Vector2::multiplyInPlace(const Vector2& otherVector)
{
  x *= otherVector.x;
  y *= otherVector.y;

  return *this;
}

Vector2 Vector2::multiply(const Vector2& otherVector) const
{
  return Vector2(x * otherVector.x, y * otherVector.y);
}

const Vector2& Vector2::multiplyToRef(const Vector2& otherVector, Vector2& result) const
{
  result.x = x * otherVector.x;
  result.y = y * otherVector.y;

  return *this;
}

Vector2 Vector2::multiplyByFloats(float ix, float iy) const
{
  return Vector2(x * ix, y * iy);
}

Vector2 Vector2::divide(const Vector2& otherVector) const
{
  return Vector2(x / otherVector.x, y / otherVector.y);
}

const Vector2& Vector2::divideToRef(const Vector2& otherVector, Vector2& result) const
{
  result.x = x / otherVector.x;
  result.y = y / otherVector.y;

  return *this;
}

Vector2& Vector2::divideInPlace(const Vector2& otherVector)
{
  x /= otherVector.x;
  y /= otherVector.y;

  return *this;
}

Vector2 Vector2::negate() const
{
  return Vector2(-x, -y);
}

Vector2& Vector2::negateInPlace()
{
  x *= -1.f;
  y *= -1.f;
  return *this;
}

Vector2 Vector2::negateToRef(Vector2& result)
{
  return result.copyFromFloats(x * -1.f, y * -1.f);
}

Vector2& Vector2::scaleInPlace(float scaleVal)
{
  x *= scaleVal;
  y *= scaleVal;

  return *this;
}

Vector2 Vector2::scale(float iScale) const
{
  Vector2 result{0.f, 0.f};
  scaleToRef(iScale, result);

  return result;
}

const Vector2& Vector2::scaleToRef(float iScale, Vector2& result) const
{
  result.x = x * iScale;
  result.y = y * iScale;

  return *this;
}

const Vector2& Vector2::scaleAndAddToRef(float iScale, Vector2& result) const
{
  result.x += x * iScale;
  result.y += y * iScale;

  return *this;
}

bool Vector2::equals(const Vector2& otherVector) const
{
  return stl_util::almost_equal(x, otherVector.x) && stl_util::almost_equal(y, otherVector.y);
}

bool Vector2::equalsWithEpsilon(const Vector2& otherVector, float epsilon) const
{
  return Scalar::WithinEpsilon(x, otherVector.x, epsilon)
         && Scalar::WithinEpsilon(y, otherVector.y, epsilon);
}

Vector2 Vector2::floor() const
{
  return Vector2(std::floor(x), std::floor(y));
}

Vector2 Vector2::fract() const
{
  return Vector2(x - std::floor(x), y - std::floor(y));
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Vector2& vector)
{
  os << "{\"X\":" << vector.x << ",\"Y\":" << vector.y << "}";

  return os;
}

Vector2 Vector2::operator+(const Vector2& otherVector) const
{
  return add(otherVector);
}

Vector2 Vector2::operator+(const Vector3& otherVector) const
{
  return addVector3(otherVector);
}

Vector2& Vector2::operator++()
{
  ++x;
  ++y;

  return *this;
}

Vector2 Vector2::operator++(int)
{
  Vector2 tmp(*this);
  operator++();
  return tmp;
}

Vector2 Vector2::operator+=(const Vector2& otherVector)
{
  return addInPlace(otherVector);
}

Vector2 Vector2::operator-(const Vector2& otherVector) const
{
  return subtract(otherVector);
}

Vector2& Vector2::operator-=(const Vector2& otherVector)
{
  return subtractInPlace(otherVector);
}

Vector2& Vector2::operator--()
{
  --x;
  --y;

  return *this;
}

Vector2 Vector2::operator--(int)
{
  Vector2 tmp(*this);
  operator--();
  return tmp;
}

Vector2 Vector2::operator*(const Vector2& otherVector) const
{
  return multiply(otherVector);
}

Vector2& Vector2::operator*=(const Vector2& otherVector)
{
  return multiplyInPlace(otherVector);
}

Vector2 Vector2::operator*(float scaleVal) const
{
  return scale(scaleVal);
}

Vector2& Vector2::operator*=(float scaleVal)
{
  return scaleInPlace(scaleVal);
}

Vector2 Vector2::operator/(const Vector2& otherVector) const
{
  return divide(otherVector);
}

Vector2& Vector2::operator/=(const Vector2& otherVector)
{
  return divideInPlace(otherVector);
}

Vector2& Vector2::operator/=(float scaleVal)
{
  const float invscaleVal = 1.0f / scaleVal;
  return scaleInPlace(invscaleVal);
}

Vector2 Vector2::operator-() const
{
  return negate();
}

bool Vector2::operator==(const Vector2& otherVector) const
{
  return equals(otherVector);
}

bool Vector2::operator!=(const Vector2& otherVector) const
{
  return !(operator==(otherVector));
}

bool Vector2::operator<(const Vector2& otherVector) const
{
  return x < otherVector.x && y < otherVector.y;
}

bool Vector2::operator>(const Vector2& otherVector) const
{
  return x > otherVector.x && y > otherVector.y;
}

bool Vector2::operator<=(const Vector2& otherVector) const
{
  return x <= otherVector.x && y <= otherVector.y;
}

bool Vector2::operator>=(const Vector2& otherVector) const
{
  return x >= otherVector.x && y >= otherVector.y;
}

/** Properties **/
float Vector2::length() const
{
  return std::sqrt(x * x + y * y);
}

float Vector2::lengthSquared() const
{
  return (x * x + y * y);
}

/** Methods **/
Vector2& Vector2::normalize()
{
  const float len = length();

  if (stl_util::almost_equal(len, 0.f)) {
    return *this;
  }

  x /= len;
  y /= len;

  return *this;
}

/** Statics **/
Vector2 Vector2::Zero()
{
  return Vector2(0.f, 0.f);
}

Vector2 Vector2::One()
{
  return Vector2(1.f, 1.f);
}

Vector2 Vector2::FromArray(const Float32Array& array, unsigned int offset)
{
  return Vector2(array[offset], array[offset + 1]);
}

void Vector2::FromArrayToRef(const Float32Array& array, unsigned int offset, Vector2& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
}

Vector2 Vector2::CatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3,
                            const Vector2& value4, float amount)
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

  return Vector2(x, y);
}

Vector2 Vector2::Clamp(const Vector2& value, const Vector2& min, const Vector2& max)
{
  float x = value.x;

  x = (x > max.x) ? max.x : x;
  x = (x < min.x) ? min.x : x;

  float y = value.y;

  y = (y > max.y) ? max.y : y;
  y = (y < min.y) ? min.y : y;

  return Vector2(x, y);
}

Vector2 Vector2::Hermite(const Vector2& value1, const Vector2& tangent1, const Vector2& value2,
                         const Vector2& tangent2, float amount)
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

  return Vector2(x, y);
}

Vector2 Vector2::Lerp(const Vector2& start, const Vector2& end, float amount)
{
  const float x = start.x + ((end.x - start.x) * amount);
  const float y = start.y + ((end.y - start.y) * amount);

  return Vector2(x, y);
}

float Vector2::Dot(const Vector2& left, const Vector2& right)
{
  return left.x * right.x + left.y * right.y;
}

Vector2 Vector2::Normalize(Vector2& vector)
{
  Vector2 newVector{vector};
  newVector.normalize();
  return newVector;
}

Vector2 Vector2::Minimize(const Vector2& left, const Vector2& right)
{
  const float x = (left.x < right.x) ? left.x : right.x;
  const float y = (left.y < right.y) ? left.y : right.y;

  return Vector2(x, y);
}

Vector2 Vector2::Maximize(const Vector2& left, const Vector2& right)
{
  const float x = (left.x > right.x) ? left.x : right.x;
  const float y = (left.y > right.y) ? left.y : right.y;

  return Vector2(x, y);
}

Vector2 Vector2::Transform(const Vector2& vector, const Matrix& transformation)
{
  Vector2 r = Vector2::Zero();
  Vector2::TransformToRef(vector, transformation, r);
  return r;
}

void Vector2::TransformToRef(const Vector2& vector, const Matrix& transformation, Vector2& result)
{
  const auto& m = transformation.m();
  const auto x  = (vector.x * m[0]) + (vector.y * m[4]) + m[12];
  const auto y  = (vector.x * m[1]) + (vector.y * m[5]) + m[13];
  result.x      = x;
  result.y      = y;
}

bool Vector2::PointInTriangle(const Vector2& p, const Vector2& p0, const Vector2& p1,
                              const Vector2& p2)
{
  const float a
    = 1.f / 2.f * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
  const float sign = a < 0.f ? -1.f : 1.f;
  const float s    = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
  const float t    = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;

  return s > 0.f && t > 0.f && (s + t) < 2.f * a * sign;
}

float Vector2::Distance(const Vector2& value1, const Vector2& value2)
{
  return std::sqrt(Vector2::DistanceSquared(value1, value2));
}

float Vector2::DistanceSquared(const Vector2& value1, const Vector2& value2)
{
  const float x = value1.x - value2.x;
  const float y = value1.y - value2.y;

  return (x * x) + (y * y);
}

Vector2 Vector2::Center(const Vector2& value1, const Vector2& value2)
{
  Vector2 center = value1.add(value2);
  center.scaleInPlace(0.5f);
  return center;
}

float Vector2::DistanceOfPointFromSegment(const Vector2& p, const Vector2& segA,
                                          const Vector2& segB)
{
  const float l2 = Vector2::DistanceSquared(segA, segB);
  if (stl_util::almost_equal(l2, 0.f)) {
    return Vector2::Distance(p, segA);
  }
  const Vector2 v    = segB.subtract(segA);
  const float t      = std::max(0.f, std::min(1.f, Vector2::Dot(p.subtract(segA), v) / l2));
  const Vector2 proj = segA.add(v.multiplyByFloats(t, t));
  return Vector2::Distance(p, proj);
}

} // end of namespace BABYLON
