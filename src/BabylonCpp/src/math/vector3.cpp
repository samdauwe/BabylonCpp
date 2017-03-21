#include <babylon/math/vector3.h>

#include <babylon/math/axis.h>
#include <babylon/math/matrix.h>
#include <babylon/math/quaternion.h>
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
    std::swap(x, otherVector.x);
    std::swap(y, otherVector.y);
    std::swap(z, otherVector.z);
  }

  return *this;
}

Vector3 Vector3::copy() const
{
  return Vector3(*this);
}

std::unique_ptr<Vector3> Vector3::clone() const
{
  return std_util::make_unique<Vector3>(*this);
}

std::ostream& operator<<(std::ostream& os, const Vector3& vector)
{
  os << "{\"X\":" << vector.x << ",\"Y\":" << vector.y << ",\"Z\":" << vector.z
     << "}";
  return os;
}

const char* Vector3::getClassName() const
{
  return "Vector3";
}

size_t Vector3::getHashCode() const
{
  float tmp = ((x * 0.397f) + y * 3.97f) + z;
  return std_util::to_bitset(tmp).to_ullong();
}

/** Operators **/
Float32Array Vector3::asArray() const
{
  return {x, y, z};
}

Vector3& Vector3::toArray(Float32Array& array, unsigned int index)
{
  array[index]     = x;
  array[index + 1] = y;
  array[index + 2] = z;

  return *this;
}

Quaternion Vector3::toQuaternion() const
{
  Quaternion result(0.f, 0.f, 0.f, 1.f);

  float cosxPlusz  = std::cos((x + z) * 0.5f);
  float sinxPlusz  = std::sin((x + z) * 0.5f);
  float coszMinusx = std::cos((z - x) * 0.5f);
  float sinzMinusx = std::sin((z - x) * 0.5f);
  float cosy       = std::cos(y * 0.5f);
  float siny       = std::sin(y * 0.5f);

  result.x = coszMinusx * siny;
  result.y = -sinzMinusx * siny;
  result.z = sinxPlusz * cosy;
  result.w = cosxPlusz * cosy;

  return result;
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

Vector3& Vector3::addToRef(const Vector3& otherVector, Vector3& result)
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

Vector3& Vector3::subtractFromFloatsToRef(float ix, float iy, float iz,
                                          Vector3& result)
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

void Vector3::scaleToRef(int iscale, Vector3& result)
{
  result.x = x * static_cast<float>(iscale);
  result.y = y * static_cast<float>(iscale);
  result.z = z * static_cast<float>(iscale);
}

void Vector3::scaleToRef(float iscale, Vector3& result)
{
  result.x = x * iscale;
  result.y = y * iscale;
  result.z = z * iscale;
}

bool Vector3::equals(const Vector3& otherVector) const
{
  return std_util::almost_equal(x, otherVector.x)
         && std_util::almost_equal(y, otherVector.y)
         && std_util::almost_equal(z, otherVector.z);
}

bool Vector3::equalsWithEpsilon(const Vector3& otherVector, float epsilon) const
{
  return MathTools::WithinEpsilon(x, otherVector.x, epsilon)
         && MathTools::WithinEpsilon(y, otherVector.y, epsilon)
         && MathTools::WithinEpsilon(z, otherVector.z, epsilon);
}

bool Vector3::equalsToFloats(float ix, float iy, float iz) const
{
  return std_util::almost_equal(x, ix) && std_util::almost_equal(y, iy)
         && std_util::almost_equal(z, iz);
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

Vector3& Vector3::multiplyToRef(const Vector3& otherVector, Vector3& result)
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

Vector3& Vector3::divideInPlace(const Vector3& otherVector)
{
  x /= otherVector.x;
  y /= otherVector.y;
  z /= otherVector.z;

  return *this;
}

Vector3 Vector3::divide(const Vector3& otherVector) const
{
  return Vector3(x / otherVector.x, y / otherVector.y, z / otherVector.z);
}

Vector3& Vector3::divideToRef(const Vector3& otherVector, Vector3& result)
{
  result.x = x / otherVector.x;
  result.y = y / otherVector.y;
  result.z = z / otherVector.z;

  return *this;
}

Vector3& Vector3::minimizeInPlace(const Vector3& other)
{
  if (other.x < x)
    x = other.x;
  if (other.y < y)
    y = other.y;
  if (other.z < z)
    z = other.z;

  return *this;
}

Vector3& Vector3::maximizeInPlace(const Vector3& other)
{
  if (other.x > x)
    x = other.x;
  if (other.y > y)
    y = other.y;
  if (other.z > z)
    z = other.z;

  return *this;
}

/** Operator overloading **/
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
  return !equals(otherVector);
}

const float& Vector3::operator[](const unsigned int index) const
{
  unsigned int _index = index % 3;
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
  float len = length();

  if (std_util::almost_equal(len, 0.f) || std_util::almost_equal(len, 1.f)) {
    return *this;
  }

  const float num = 1.f / len;

  x *= num;
  y *= num;
  z *= num;

  return *this;
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

/** Statics **/
float Vector3::GetClipFactor(const Vector3& vector0, const Vector3& vector1,
                             const Vector3& axis, float size)
{
  float d0 = Vector3::Dot(vector0, axis) - size;
  float d1 = Vector3::Dot(vector1, axis) - size;

  float s = d0 / (d0 - d1);

  return s;
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

Vector3 Vector3::Up()
{
  return Vector3(0.f, 1.f, 0.f);
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
  float x = (vector.x * transformation.m[0]) + (vector.y * transformation.m[4])
            + (vector.z * transformation.m[8]) + transformation.m[12];
  float y = (vector.x * transformation.m[1]) + (vector.y * transformation.m[5])
            + (vector.z * transformation.m[9]) + transformation.m[13];
  float z = (vector.x * transformation.m[2]) + (vector.y * transformation.m[6])
            + (vector.z * transformation.m[10]) + transformation.m[14];
  float w = (vector.x * transformation.m[3]) + (vector.y * transformation.m[7])
            + (vector.z * transformation.m[11]) + transformation.m[15];

  result.x = x / w;
  result.y = y / w;
  result.z = z / w;
}

void Vector3::TransformCoordinatesFromFloatsToRef(float x, float y, float z,
                                                  const Matrix& transformation,
                                                  Vector3& result)
{
  float rx = (x * transformation.m[0]) + (y * transformation.m[4])
             + (z * transformation.m[8]) + transformation.m[12];
  float ry = (x * transformation.m[1]) + (y * transformation.m[5])
             + (z * transformation.m[9]) + transformation.m[13];
  float rz = (x * transformation.m[2]) + (y * transformation.m[6])
             + (z * transformation.m[10]) + transformation.m[14];
  float rw = (x * transformation.m[3]) + (y * transformation.m[7])
             + (z * transformation.m[11]) + transformation.m[15];

  result.x = rx / rw;
  result.y = ry / rw;
  result.z = rz / rw;
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
  float squared = amount * amount;
  float cubed   = amount * squared;

  float x
    = 0.5f
      * ((((2.f * value2.x) + ((-value1.x + value3.x) * amount))
          + (((((2.f * value1.x) - (5.f * value2.x)) + (4.f * value3.x))
              - value4.x)
             * squared))
         + ((((-value1.x + (3.f * value2.x)) - (3.f * value3.x)) + value4.x)
            * cubed));

  float y
    = 0.5f
      * ((((2.f * value2.y) + ((-value1.y + value3.y) * amount))
          + (((((2.f * value1.y) - (5.f * value2.y)) + (4.f * value3.y))
              - value4.y)
             * squared))
         + ((((-value1.y + (3.f * value2.y)) - (3.f * value3.y)) + value4.y)
            * cubed));

  float z
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
  float squared = amount * amount;
  float cubed   = amount * squared;
  float part1   = ((2.f * cubed) - (3.f * squared)) + 1.f;
  float part2   = (-2.f * cubed) + (3.f * squared);
  float part3   = (cubed - (2.f * squared)) + amount;
  float part4   = cubed - squared;

  float x = (((value1.x * part1) + (value2.x * part2)) + (tangent1.x * part3))
            + (tangent2.x * part4);
  float y = (((value1.y * part1) + (value2.y * part2)) + (tangent1.y * part3))
            + (tangent2.y * part4);
  float z = (((value1.z * part1) + (value2.z * part2)) + (tangent1.z * part3))
            + (tangent2.z * part4);

  return Vector3(x, y, z);
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float amount)
{
  float x = start.x + ((end.x - start.x) * amount);
  float y = start.y + ((end.y - start.y) * amount);
  float z = start.z + ((end.z - start.z) * amount);

  return Vector3(x, y, z);
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
  result.x = left.y * right.z - left.z * right.y;
  result.y = left.z * right.x - left.x * right.z;
  result.z = left.x * right.y - left.y * right.x;
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
  float cw = static_cast<float>(viewport.width);
  float ch = static_cast<float>(viewport.height);
  float cx = static_cast<float>(viewport.x);
  float cy = static_cast<float>(viewport.y);

  Matrix viewportMatrix
    = Matrix::FromValues(cw / 2.f, 0.f, 0.f, 0.f, 0.f, -ch / 2.f, 0.f, 0.f, 0.f,
                         0.f, 1.f, 0.f, cx + cw / 2.f, ch / 2.f + cy, 0.f, 1.f);

  Matrix finalMatrix = world.multiply(transform).multiply(viewportMatrix);

  return Vector3::TransformCoordinates(vector, finalMatrix);
}

Vector3 Vector3::UnprojectFromTransform(Vector3& source, float viewportWidth,
                                        float viewportHeight, Matrix& world,
                                        Matrix& transform)
{
  Matrix matrix = world.multiply(transform);
  matrix.invert();
  source.x       = source.x / viewportWidth * 2 - 1;
  source.y       = -(source.y / viewportHeight * 2 - 1);
  Vector3 vector = Vector3::TransformCoordinates(source, matrix);
  float num      = source.x * matrix.m[3] + source.y * matrix.m[7]
              + source.z * matrix.m[11] + matrix.m[15];

  if (MathTools::WithinEpsilon(num, 1.f)) {
    vector = vector.scale(1.f / num);
  }

  return vector;
}

Vector3 Vector3::Unproject(const Vector3& source, float viewportWidth,
                           float viewportHeight, Matrix& world, Matrix& view,
                           Matrix& projection)
{
  Matrix matrix = world.multiply(view).multiply(projection);
  matrix.invert();
  Vector3 screenSource(source.x / viewportWidth * 2.f - 1,
                       -(source.y / viewportHeight * 2.f - 1), source.z);
  Vector3 vector = Vector3::TransformCoordinates(screenSource, matrix);
  float num      = screenSource.x * matrix.m[3] + screenSource.y * matrix.m[7]
              + screenSource.z * matrix.m[11] + matrix.m[15];

  if (MathTools::WithinEpsilon(num, 1.f)) {
    vector = vector.scale(1.f / num);
  }

  return vector;
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
  float x = value1.x - value2.x;
  float y = value1.y - value2.y;
  float z = value1.z - value2.z;

  return (x * x) + (y * y) + (z * z);
}

Vector3 Vector3::Center(const Vector3& value1, const Vector3& value2)
{
  Vector3 center = value1.add(value2);
  center.scaleInPlace(0.5f);
  return center;
}

/**
 * Given three orthogonal left-handed oriented Vector3 axis in space (target
 * system),
 * RotationFromAxis() returns the rotation Euler angles (ex : rotation.x,
 * rotation.y, rotation.z)
 * to apply to something in order to rotate it from its local system to the
 * given target system.
 */
Vector3 Vector3::RotationFromAxis(const Vector3& axis1, const Vector3& axis2,
                                  const Vector3& axis3)
{
  Vector3 rotation = Vector3::Zero();
  Vector3::RotationFromAxisToRef(axis1, axis2, axis3, rotation);
  return rotation;
}

/**
 * The same than RotationFromAxis but updates the passed ref Vector3
 * parameter.
 */
void Vector3::RotationFromAxisToRef(const Vector3& axis1, const Vector3& axis2,
                                    const Vector3& axis3, Vector3& ref)
{
  Vector3 u = Vector3::Normalize(axis1);
  Vector3 w = Vector3::Normalize(axis2);
  w         = Vector3::Normalize(axis3);

  // world axis
  Vector3 X = Axis::X;
  Vector3 Y = Axis::Y;

  // equation unknowns and vars
  float yaw    = 0.f;
  float pitch  = 0.f;
  float roll   = 0.f;
  float x      = 0.f;
  float y      = 0.f;
  float z      = 0.f;
  float t      = 0.f;
  float sign   = -1.f;
  int nbRevert = 0;
  Vector3 cross;
  float dot = 0.f;

  // step 1  : rotation around w
  // Rv3(u) = u1, and u1 belongs to plane xOz
  // Rv3(w) = w1 = w invariant
  if (MathTools::WithinEpsilon(w.z, 0.f)) {
    z = 1.f;
  }
  else if (MathTools::WithinEpsilon(w.x, 0.f)) {
    x = 1.f;
  }
  else {
    t = w.z / w.x;
    x = -t * std::sqrt(1.f / (1.f + t * t));
    z = std::sqrt(1.f / (1.f + t * t));
  }

  Vector3 u1(x, y, z);
  u1.normalize();
  Vector3::CrossToRef(u, u1, cross); // returns same direction as w (=local z)
                                     // if positive angle : cross(source, image)
  cross.normalize();
  if (Vector3::Dot(w, cross) < 0.f) {
    sign = 1.f;
  }

  dot = Vector3::Dot(u, u1);
  dot = std::min(
    1.f, std::max(-1.f, dot)); // to force dot to be in the range [-1, 1]
  roll = std::acos(dot) * sign;

  if (Vector3::Dot(u1, X) < 0.f) { // checks X orientation
    roll = Math::PI + roll;
    u1   = u1.scaleInPlace(-1.f);
    ++nbRevert;
  }

  // step 2 : rotate around u1
  // Ru1(w1) = Ru1(w) = w2, and w2 belongs to plane xOz
  // u1 is yet in xOz and invariant by Ru1, so after this step u1 and w2 will be
  // in xOz
  Vector3 v2;
  x    = 0.f;
  y    = 0.f;
  z    = 0.f;
  sign = -1.f;
  if (MathTools::WithinEpsilon(w.z, 0.f)) {
    x = 1.f;
  }
  else {
    t = u1.z / u1.x;
    x = -t * std::sqrt(1.f / (1.f + t * t));
    z = std::sqrt(1.f / (1.f + t * t));
  }

  Vector3 w2(x, y, z);
  w2.normalize();
  Vector3::CrossToRef(w2, u1, v2); // v2 image of v1 through rotation around u1
  v2.normalize();
  Vector3::CrossToRef(w, w2, cross); // returns same direction as u1 (=local x)
                                     // if positive angle : cross(source, image)
  cross.normalize();
  if (Vector3::Dot(u1, cross) < 0.f) {
    sign = 1.f;
  }

  dot = Vector3::Dot(w, w2);
  dot = std::min(
    1.f, std::max(-1.f, dot)); // to force dot to be in the range [-1, 1]
  pitch = std::acos(dot) * sign;
  if (Vector3::Dot(v2, Y) < 0.f) { // checks for Y orientation
    pitch = Math::PI + pitch;
    ++nbRevert;
  }

  // step 3 : rotate around v2
  // Rv2(u1) = X, same as Rv2(w2) = Z, with X=(1,0,0) and Z=(0,0,1)
  sign = -1.f;
  Vector3::CrossToRef(X, u1, cross); // returns same direction as Y if positive
                                     // angle : cross(source, image)
  cross.normalize();
  if (Vector3::Dot(cross, Y) < 0.f) {
    sign = 1.f;
  }
  dot = Vector3::Dot(u1, X);
  // to force dot to be in the range [-1, 1]
  dot = (std::min(1.f, std::max(-1.f, dot)));
  yaw = -std::acos(dot) * sign; // negative : plane zOx oriented clockwise
  if (dot < 0.f && static_cast<float>(nbRevert) < 2.f) {
    yaw = Math::PI + yaw;
  }

  ref.x = pitch;
  ref.y = yaw;
  ref.z = roll;
}

} // end of namespace BABYLON
