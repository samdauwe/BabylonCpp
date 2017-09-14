#include <babylon/math/quaternion.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/math_tmp.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

Quaternion::Quaternion(float ix, float iy, float iz, float iw)
    : x{ix}, y{iy}, z{iz}, w{iw}
{
}

Quaternion::Quaternion(const Quaternion& otherQuaternion)
    : x{otherQuaternion.x}
    , y{otherQuaternion.y}
    , z{otherQuaternion.z}
    , w{otherQuaternion.w}
{
}

Quaternion::Quaternion(Quaternion&& otherQuaternion)
    : x{::std::move(otherQuaternion.x)}
    , y{::std::move(otherQuaternion.y)}
    , z{::std::move(otherQuaternion.z)}
    , w{::std::move(otherQuaternion.w)}
{
}

Quaternion& Quaternion::operator=(const Quaternion& otherQuaternion)
{
  if (&otherQuaternion != this) {
    x = otherQuaternion.x;
    y = otherQuaternion.y;
    z = otherQuaternion.z;
    w = otherQuaternion.w;
  }

  return *this;
}

Quaternion& Quaternion::operator=(Quaternion&& otherQuaternion)
{
  if (&otherQuaternion != this) {
    x = ::std::move(otherQuaternion.x);
    y = ::std::move(otherQuaternion.y);
    z = ::std::move(otherQuaternion.z);
    w = ::std::move(otherQuaternion.w);
  }

  return *this;
}

Quaternion::~Quaternion()
{
}

Quaternion Quaternion::copy() const
{
  return Quaternion(*this);
}

std::unique_ptr<Quaternion> Quaternion::clone() const
{
  return ::std::make_unique<Quaternion>(*this);
}

string_t Quaternion::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

const char* Quaternion::getClassName() const
{
  return "Quaternion";
}

int Quaternion::getHashCode() const
{
  float hash = x;
  hash       = ::std::pow((hash * 397), y);
  hash       = ::std::pow((hash * 397), z);
  hash       = ::std::pow((hash * 397), w);
  return static_cast<int>(hash);
}

/** Methods **/
Float32Array Quaternion::asArray() const
{
  return {x, y, z, w};
}

bool Quaternion::equals(const Quaternion& otherQuaternion) const
{
  return stl_util::almost_equal(x, otherQuaternion.x)
         && stl_util::almost_equal(y, otherQuaternion.y)
         && stl_util::almost_equal(z, otherQuaternion.z)
         && stl_util::almost_equal(w, otherQuaternion.w);
}

Quaternion& Quaternion::copyFrom(const Quaternion& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;

  return *this;
}

Quaternion& Quaternion::copyFromFloats(float ix, float iy, float iz, float iw)
{
  x = ix;
  y = iy;
  z = iz;
  w = iw;

  return *this;
}

Quaternion& Quaternion::set(float x, float y, float z, float w)
{
  return copyFromFloats(x, y, z, w);
}

Quaternion Quaternion::add(const Quaternion& other) const
{
  return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
}

Quaternion Quaternion::subtract(const Quaternion& other) const
{
  return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
}

Quaternion Quaternion::scale(float value) const
{
  return Quaternion(x * value, y * value, z * value, w * value);
}

Quaternion Quaternion::multiply(const Quaternion& q1) const
{
  Quaternion result(0.f, 0.f, 0.f, 1.f);

  multiplyToRef(q1, result);

  return result;
}

const Quaternion& Quaternion::multiplyToRef(const Quaternion& q1,
                                            Quaternion& result) const
{
  const float xTemp = x * q1.w + y * q1.z - z * q1.y + w * q1.x;
  const float yTemp = -x * q1.z + y * q1.w + z * q1.x + w * q1.y;
  const float zTemp = x * q1.y - y * q1.x + z * q1.w + w * q1.z;
  const float wTemp = -x * q1.x - y * q1.y - z * q1.z + w * q1.w;
  result.copyFromFloats(xTemp, yTemp, zTemp, wTemp);

  return *this;
}

Quaternion& Quaternion::multiplyInPlace(const Quaternion& q1)
{
  multiplyToRef(q1, *this);

  return *this;
}

const Quaternion& Quaternion::conjugateToRef(Quaternion& ref) const
{
  ref.copyFromFloats(-x, -y, -z, w);
  return *this;
}

Quaternion& Quaternion::conjugateInPlace()
{
  x *= -1.f;
  y *= -1.f;
  z *= -1.f;
  return *this;
}

Quaternion Quaternion::conjugate() const
{
  return Quaternion(-x, -y, -z, w);
}

float Quaternion::length() const
{
  return ::std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

Quaternion& Quaternion::normalize()
{
  const float val = 1.f / length();
  x *= val;
  y *= val;
  z *= val;
  w *= val;

  return *this;
}

Vector3 Quaternion::toEulerAngles() const
{
  Vector3 result = Vector3::Zero();

  toEulerAnglesToRef(result);

  return result;
}

const Quaternion&
Quaternion::toEulerAnglesToRef(Vector3& result, const string_t& /*order*/) const
{
  const float qz = z;
  const float qx = x;
  const float qy = y;
  const float qw = w;

  const float sqw = qw * qw;
  const float sqz = qz * qz;
  const float sqx = qx * qx;
  const float sqy = qy * qy;

  const float zAxisY = qy * qz - qx * qw;
  const float limit  = 0.4999999f;

  if (zAxisY < -limit) {
    result.y = 2.f * ::std::atan2(qy, qw);
    result.x = Math::PI_2;
    result.z = 0.f;
  }
  else if (zAxisY > limit) {
    result.y = 2.f * ::std::atan2(qy, qw);
    result.x = -Math::PI_2;
    result.z = 0.f;
  }
  else {
    result.z
      = ::std::atan2(2.f * (qx * qy + qz * qw), (-sqz - sqx + sqy + sqw));
    result.x = ::std::asin(-2.f * (qz * qy - qx * qw));
    result.y = ::std::atan2(2.f * (qz * qx + qy * qw), (sqz - sqx - sqy + sqw));
  }

  return *this;
}

const Quaternion& Quaternion::toRotationMatrix(Matrix& result) const
{
  const float xx = x * x;
  const float yy = y * y;
  const float zz = z * z;
  const float xy = x * y;
  const float zw = z * w;
  const float zx = z * x;
  const float yw = y * w;
  const float yz = y * z;
  const float xw = x * w;

  result.m[0]  = 1.f - (2.f * (yy + zz));
  result.m[1]  = 2.f * (xy + zw);
  result.m[2]  = 2.f * (zx - yw);
  result.m[3]  = 0;
  result.m[4]  = 2.f * (xy - zw);
  result.m[5]  = 1.f - (2.f * (zz + xx));
  result.m[6]  = 2.f * (yz + xw);
  result.m[7]  = 0;
  result.m[8]  = 2.f * (zx + yw);
  result.m[9]  = 2.f * (yz - xw);
  result.m[10] = 1.f - (2.f * (yy + xx));
  result.m[11] = 0;
  result.m[12] = 0;
  result.m[13] = 0;
  result.m[14] = 0;
  result.m[15] = 1.f;

  result._markAsUpdated();

  return *this;
}

Quaternion& Quaternion::fromRotationMatrix(const Matrix& matrix)
{
  Quaternion::FromRotationMatrixToRef(matrix, *this);
  return *this;
}

/** Operator overloading **/
std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion)
{
  os << "{\"X\":" << quaternion.x << ",\"Y\":" << quaternion.y
     << ",\"Z\":" << quaternion.z << ",\"W\":" << quaternion.w << "}";
  return os;
}

Quaternion Quaternion::operator+(const Quaternion& other) const
{
  return add(other);
}

Quaternion Quaternion::operator-(const Quaternion& other) const
{
  return subtract(other);
}

Quaternion Quaternion::operator*(float value) const
{
  return scale(value);
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{
  return multiply(other);
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
  return multiplyInPlace(other);
}

bool Quaternion::operator==(const Quaternion& other) const
{
  return equals(other);
}

bool Quaternion::operator!=(const Quaternion& other) const
{
  return !(operator==(other));
}

/** Statics **/
Quaternion Quaternion::FromRotationMatrix(const Matrix& matrix)
{
  Quaternion result;
  Quaternion::FromRotationMatrixToRef(matrix, result);
  return result;
}

void Quaternion::FromRotationMatrixToRef(const Matrix& matrix,
                                         Quaternion& result)
{
  const array_t<float, 16>& data = matrix.m;
  const float m11 = data[0], m12 = data[4], m13 = data[8];
  const float m21 = data[1], m22 = data[5], m23 = data[9];
  const float m31 = data[2], m32 = data[6], m33 = data[10];
  const float trace = m11 + m22 + m33;
  float s;

  if (trace > 0.f) {
    s = 0.5f / ::std::sqrt(trace + 1.f);

    result.w = 0.25f / s;
    result.x = (m32 - m23) * s;
    result.y = (m13 - m31) * s;
    result.z = (m21 - m12) * s;
  }
  else if (m11 > m22 && m11 > m33) {
    s = 2.f * ::std::sqrt(1.f + m11 - m22 - m33);

    result.w = (m32 - m23) / s;
    result.x = 0.25f * s;
    result.y = (m12 + m21) / s;
    result.z = (m13 + m31) / s;
  }
  else if (m22 > m33) {
    s = 2.f * ::std::sqrt(1.f + m22 - m11 - m33);

    result.w = (m13 - m31) / s;
    result.x = (m12 + m21) / s;
    result.y = 0.25f * s;
    result.z = (m23 + m32) / s;
  }
  else {
    s = 2.f * ::std::sqrt(1.f + m33 - m11 - m22);

    result.w = (m21 - m12) / s;
    result.x = (m13 + m31) / s;
    result.y = (m23 + m32) / s;
    result.z = 0.25f * s;
  }
}

Quaternion Quaternion::Zero()
{
  return Quaternion(0.f, 0.f, 0.f, 0.f);
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
  return Quaternion(-q.x, -q.y, -q.z, q.w);
}

Quaternion Quaternion::Identity()
{
  return Quaternion(0.f, 0.f, 0.f, 1.f);
}

bool Quaternion::IsIdentity(const Quaternion& quaternion)
{
  return quaternion.x == 0.f && quaternion.y == 0.f && quaternion.z == 0.f
         && quaternion.w == 1.f;
}

Quaternion Quaternion::RotationAxis(Vector3& axis, float angle)
{
  Quaternion result;
  return Quaternion::RotationAxisToRef(axis, angle, result);
}

Quaternion Quaternion::RotationAxisToRef(Vector3& axis, float angle,
                                         Quaternion& result)
{
  const float _sin = ::std::sin(angle / 2.f);

  axis.normalize();

  result.w = ::std::cos(angle / 2.f);
  result.x = axis.x * _sin;
  result.y = axis.y * _sin;
  result.z = axis.z * _sin;

  return result;
}

Quaternion Quaternion::FromArray(const Float32Array& array, unsigned int offset)
{
  return Quaternion(array[offset], array[offset + 1], array[offset + 2],
                    array[offset + 3]);
}

Quaternion Quaternion::RotationYawPitchRoll(float yaw, float pitch, float roll)
{
  Quaternion result;

  Quaternion::RotationYawPitchRollToRef(yaw, pitch, roll, result);

  return result;
}

void Quaternion::RotationYawPitchRollToRef(float yaw, float pitch, float roll,
                                           Quaternion& result)
{
  // Produces a quaternion from Euler angles in the z-y-x orientation
  // (Tait-Bryan angles)
  const float halfRoll  = roll * 0.5f;
  const float halfPitch = pitch * 0.5f;
  const float halfYaw   = yaw * 0.5f;

  const float sinRoll  = ::std::sin(halfRoll);
  const float cosRoll  = ::std::cos(halfRoll);
  const float sinPitch = ::std::sin(halfPitch);
  const float cosPitch = ::std::cos(halfPitch);
  const float sinYaw   = ::std::sin(halfYaw);
  const float cosYaw   = ::std::cos(halfYaw);

  result.x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
  result.y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
  result.z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
  result.w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
}

Quaternion Quaternion::RotationAlphaBetaGamma(float alpha, float beta,
                                              float gamma)
{
  Quaternion result;
  Quaternion::RotationAlphaBetaGammaToRef(alpha, beta, gamma, result);
  return result;
}

void Quaternion::RotationAlphaBetaGammaToRef(float alpha, float beta,
                                             float gamma, Quaternion& result)
{
  // Produces a quaternion from Euler angles in the z-x-z orientation
  const float halfGammaPlusAlpha  = (gamma + alpha) * 0.5f;
  const float halfGammaMinusAlpha = (gamma - alpha) * 0.5f;
  const float halfBeta            = beta * 0.5f;

  result.x = ::std::cos(halfGammaMinusAlpha) * ::std::sin(halfBeta);
  result.y = ::std::sin(halfGammaMinusAlpha) * ::std::sin(halfBeta);
  result.z = ::std::sin(halfGammaPlusAlpha) * ::std::cos(halfBeta);
  result.w = ::std::cos(halfGammaPlusAlpha) * ::std::cos(halfBeta);
}

Quaternion Quaternion::RotationQuaternionFromAxis(Vector3& axis1,
                                                  Vector3& axis2,
                                                  Vector3& axis3)
{
  Quaternion quat(0.f, 0.f, 0.f, 0.f);
  Quaternion::RotationQuaternionFromAxisToRef(axis1, axis2, axis3, quat);
  return quat;
}

void Quaternion::RotationQuaternionFromAxisToRef(Vector3& axis1, Vector3& axis2,
                                                 Vector3& axis3,
                                                 Quaternion& ref)
{
  auto& rotMat = MathTmp::MatrixArray[0];
  Matrix::FromXYZAxesToRef(axis1.normalize(), axis2.normalize(),
                           axis3.normalize(), rotMat);
  Quaternion::FromRotationMatrixToRef(rotMat, ref);
}

Quaternion Quaternion::Slerp(const Quaternion& left, const Quaternion& right,
                             float amount)
{
  Quaternion result = Quaternion::Identity();

  Quaternion::SlerpToRef(left, right, amount, result);

  return result;
}

void Quaternion::SlerpToRef(const Quaternion& left, const Quaternion& right,
                            float amount, Quaternion& result)
{
  float num2, num3, num = amount;
  float num4 = (((left.x * right.x) + (left.y * right.y)) + (left.z * right.z))
               + (left.w * right.w);
  bool flag = false;

  if (num4 < 0.f) {
    flag = true;
    num4 = -num4;
  }

  if (num4 > 0.999999f) {
    num3 = 1.f - num;
    num2 = flag ? -num : num;
  }
  else {
    const float num5 = ::std::acos(num4);
    const float num6 = (1.f / ::std::sin(num5));
    num3             = (::std::sin((1.f - num) * num5)) * num6;
    num2             = flag ? ((-::std::sin(num * num5)) * num6) :
                  ((::std::sin(num * num5)) * num6);
  }

  result.x = (num3 * left.x) + (num2 * right.x);
  result.y = (num3 * left.y) + (num2 * right.y);
  result.z = (num3 * left.z) + (num2 * right.z);
  result.w = (num3 * left.w) + (num2 * right.w);
}

Quaternion Quaternion::Hermite(const Quaternion& value1,
                               const Quaternion& tangent1,
                               const Quaternion& value2,
                               const Quaternion& tangent2, float amount)
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
  const float w
    = (((value1.w * part1) + (value2.w * part2)) + (tangent1.w * part3))
      + (tangent2.w * part4);

  return Quaternion(x, y, z, w);
}

} // end of namespace BABYLON
