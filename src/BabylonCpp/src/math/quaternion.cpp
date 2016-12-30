#include <babylon/math/quaternion.h>

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
    : x{std::move(otherQuaternion.x)}
    , y{std::move(otherQuaternion.y)}
    , z{std::move(otherQuaternion.z)}
    , w{std::move(otherQuaternion.w)}
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
    std::swap(x, otherQuaternion.x);
    std::swap(y, otherQuaternion.y);
    std::swap(z, otherQuaternion.z);
    std::swap(w, otherQuaternion.w);
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
  return std_util::make_unique<Quaternion>(*this);
}

std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion)
{
  os << "{\"X\":" << quaternion.x << ",\"Y\":" << quaternion.y
     << ",\"Z\":" << quaternion.z << ",\"W\":" << quaternion.w << "}";
  return os;
}

const char* Quaternion::getClassName() const
{
  return "Quaternion";
}

int Quaternion::getHashCode() const
{
  float hash = x;
  hash       = std::pow((hash * 397), y);
  hash       = std::pow((hash * 397), z);
  hash       = std::pow((hash * 397), w);
  return static_cast<int>(hash);
}

/** Methods **/
Float32Array Quaternion::asArray() const
{
  return {x, y, z, w};
}

bool Quaternion::equals(const Quaternion& otherQuaternion) const
{
  return std_util::almost_equal(x, otherQuaternion.x)
         && std_util::almost_equal(y, otherQuaternion.y)
         && std_util::almost_equal(z, otherQuaternion.z)
         && std_util::almost_equal(w, otherQuaternion.w);
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

Quaternion Quaternion::multiply(const Quaternion& q1)
{
  Quaternion result = Quaternion(0.f, 0.f, 0.f, 1.f);

  multiplyToRef(q1, result);

  return result;
}

Quaternion& Quaternion::multiplyToRef(const Quaternion& q1, Quaternion& result)
{
  float xTemp = x * q1.w + y * q1.z - z * q1.y + w * q1.x;
  float yTemp = -x * q1.z + y * q1.w + z * q1.x + w * q1.y;
  float zTemp = x * q1.y - y * q1.x + z * q1.w + w * q1.z;
  float wTemp = -x * q1.x - y * q1.y - z * q1.z + w * q1.w;
  result.copyFromFloats(xTemp, yTemp, zTemp, wTemp);

  return *this;
}

Quaternion& Quaternion::multiplyInPlace(const Quaternion& q1)
{
  multiplyToRef(q1, *this);

  return *this;
}

Quaternion& Quaternion::conjugateToRef(Quaternion& ref)
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
  return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

Quaternion& Quaternion::normalize()
{
  float val = 1.f / length();
  x *= val;
  y *= val;
  z *= val;
  w *= val;

  return *this;
}

Vector3 Quaternion::toEulerAngles()
{
  Vector3 result = Vector3::Zero();

  toEulerAnglesToRef(result);

  return result;
}

Quaternion& Quaternion::toEulerAnglesToRef(Vector3& result,
                                           const std::string& /*order*/)
{
  float qz = z;
  float qx = x;
  float qy = y;
  float qw = w;

  float sqw = qw * qw;
  float sqz = qz * qz;
  float sqx = qx * qx;
  float sqy = qy * qy;

  float zAxisY = qy * qz - qx * qw;
  float limit  = 0.4999999f;

  if (zAxisY < -limit) {
    result.y = 2.f * std::atan2(qy, qw);
    result.x = Math::PI_2;

    result.z = 0.f;
  }
  else if (zAxisY > limit) {
    result.y = 2.f * std::atan2(qy, qw);
    result.x = -Math::PI_2;
    result.z = 0.f;
  }
  else {
    result.z = std::atan2(2.f * (qx * qy + qz * qw), (-sqz - sqx + sqy + sqw));
    result.x = std::asin(-2.f * (qz * qy - qx * qw));
    result.y = std::atan2(2.f * (qz * qx + qy * qw), (sqz - sqx - sqy + sqw));
  }

  return *this;
}

Quaternion& Quaternion::toRotationMatrix(Matrix& result)
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

  return *this;
}

Quaternion& Quaternion::fromRotationMatrix(const Matrix& matrix)
{
  Quaternion::FromRotationMatrixToRef(matrix, *this);
  return *this;
}

/** Operator overloading **/
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

Quaternion Quaternion::operator*(const Quaternion& other)
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
  return !equals(other);
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
  const std::array<float, 16>& data = matrix.m;
  float m11 = data[0], m12 = data[4], m13 = data[8];
  float m21 = data[1], m22 = data[5], m23 = data[9];
  float m31 = data[2], m32 = data[6], m33 = data[10];
  float trace = m11 + m22 + m33;
  float s;

  if (trace > 0.f) {
    s = 0.5f / std::sqrt(trace + 1.f);

    result.w = 0.25f / s;
    result.x = (m32 - m23) * s;
    result.y = (m13 - m31) * s;
    result.z = (m21 - m12) * s;
  }
  else if (m11 > m22 && m11 > m33) {
    s = 2.f * std::sqrt(1.f + m11 - m22 - m33);

    result.w = (m32 - m23) / s;
    result.x = 0.25f * s;
    result.y = (m12 + m21) / s;
    result.z = (m13 + m31) / s;
  }
  else if (m22 > m33) {
    s = 2.f * std::sqrt(1.f + m22 - m11 - m33);

    result.w = (m13 - m31) / s;
    result.x = (m12 + m21) / s;
    result.y = 0.25f * s;
    result.z = (m23 + m32) / s;
  }
  else {
    s = 2.f * std::sqrt(1.f + m33 - m11 - m22);

    result.w = (m21 - m12) / s;
    result.x = (m13 + m31) / s;
    result.y = (m23 + m32) / s;
    result.z = 0.25f * s;
  }
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
  return Quaternion(-q.x, -q.y, -q.z, q.w);
}

Quaternion Quaternion::Identity()
{
  return Quaternion(0.f, 0.f, 0.f, 1.f);
}

Quaternion Quaternion::RotationAxis(Vector3& axis, float angle)
{
  Quaternion result;
  return Quaternion::RotationAxisToRef(axis, angle, result);
}

Quaternion Quaternion::RotationAxisToRef(Vector3& axis, float angle,
                                         Quaternion& result)
{
  float _sin = std::sin(angle / 2.f);

  axis.normalize();

  result.w = std::cos(angle / 2.f);
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
  float halfRoll  = roll * 0.5f;
  float halfPitch = pitch * 0.5f;
  float halfYaw   = yaw * 0.5f;

  float sinRoll  = std::sin(halfRoll);
  float cosRoll  = std::cos(halfRoll);
  float sinPitch = std::sin(halfPitch);
  float cosPitch = std::cos(halfPitch);
  float sinYaw   = std::sin(halfYaw);
  float cosYaw   = std::cos(halfYaw);

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
  float halfGammaPlusAlpha  = (gamma + alpha) * 0.5f;
  float halfGammaMinusAlpha = (gamma - alpha) * 0.5f;
  float halfBeta            = beta * 0.5f;

  result.x = std::cos(halfGammaMinusAlpha) * std::sin(halfBeta);
  result.y = std::sin(halfGammaMinusAlpha) * std::sin(halfBeta);
  result.z = std::sin(halfGammaPlusAlpha) * std::cos(halfBeta);
  result.w = std::cos(halfGammaPlusAlpha) * std::cos(halfBeta);
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
    float num5 = std::acos(num4);
    float num6 = (1.f / std::sin(num5));
    num3       = (std::sin((1.f - num) * num5)) * num6;
    num2       = flag ? ((-std::sin(num * num5)) * num6) :
                  ((std::sin(num * num5)) * num6);
  }

  result.x = (num3 * left.x) + (num2 * right.x);
  result.y = (num3 * left.y) + (num2 * right.y);
  result.z = (num3 * left.z) + (num2 * right.z);
  result.w = (num3 * left.w) + (num2 * right.w);
}

} // end of namespace BABYLON
