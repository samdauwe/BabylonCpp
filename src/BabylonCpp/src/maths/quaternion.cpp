#include <babylon/maths/quaternion.h>

#include <cmath>

#include <babylon/babylon_stl_util.h>
#include <babylon/maths/math_tmp.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/scalar.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

Quaternion::Quaternion(float ix, float iy, float iz, float iw) : x{ix}, y{iy}, z{iz}, w{iw}
{
}

Quaternion::Quaternion(const Quaternion& otherQuaternion) = default;

Quaternion::Quaternion(Quaternion&& otherQuaternion) = default;

Quaternion& Quaternion::operator=(const Quaternion& otherQuaternion) = default;

Quaternion& Quaternion::operator=(Quaternion&& otherQuaternion) = default;

Quaternion::~Quaternion() = default;

Quaternion Quaternion::copy() const
{
  return Quaternion(*this);
}

std::unique_ptr<Quaternion> Quaternion::clone() const
{
  return std::make_unique<Quaternion>(*this);
}

std::string Quaternion::toString() const
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
  return stl_util::almost_equal(x, otherQuaternion.x)
         && stl_util::almost_equal(y, otherQuaternion.y)
         && stl_util::almost_equal(z, otherQuaternion.z)
         && stl_util::almost_equal(w, otherQuaternion.w);
}

bool Quaternion::equalsWithEpsilon(const Quaternion& otherQuaternion, float epsilon) const
{
  return Scalar::WithinEpsilon(x, otherQuaternion.x, epsilon)
         && Scalar::WithinEpsilon(y, otherQuaternion.y, epsilon)
         && Scalar::WithinEpsilon(z, otherQuaternion.z, epsilon)
         && Scalar::WithinEpsilon(w, otherQuaternion.w, epsilon);
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

Quaternion& Quaternion::set(float ix, float iy, float iz, float iw)
{
  return copyFromFloats(ix, iy, iz, iw);
}

Quaternion Quaternion::add(const Quaternion& other) const
{
  return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
}

Quaternion& Quaternion::addInPlace(const Quaternion& other)
{
  x += other.x;
  y += other.y;
  z += other.z;
  w += other.w;
  return *this;
}

Quaternion Quaternion::subtract(const Quaternion& other) const
{
  return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
}

Quaternion Quaternion::scale(float value) const
{
  return Quaternion(x * value, y * value, z * value, w * value);
}

const Quaternion& Quaternion::scaleToRef(float iscale, Quaternion& result) const
{
  result.x = x * iscale;
  result.y = y * iscale;
  result.z = z * iscale;
  result.w = w * iscale;

  return *this;
}

Quaternion& Quaternion::scaleInPlace(float value)
{
  x *= value;
  y *= value;
  z *= value;
  w *= value;

  return *this;
}

const Quaternion& Quaternion::scaleAndAddToRef(float iscale, Quaternion& result) const
{
  result.x += x * iscale;
  result.y += y * iscale;
  result.z += z * iscale;
  result.w += w * iscale;

  return *this;
}

Quaternion Quaternion::multiply(const Quaternion& q1) const
{
  Quaternion result(0.f, 0.f, 0.f, 1.f);

  multiplyToRef(q1, result);

  return result;
}

const Quaternion& Quaternion::multiplyToRef(const Quaternion& q1, Quaternion& result) const
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
  return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

Quaternion& Quaternion::normalize()
{
  const auto len = length();

  if (len == 0.f) {
    return *this;
  }

  const auto inv = 1.f / len;
  x *= inv;
  y *= inv;
  z *= inv;
  w *= inv;

  return *this;
}

Vector3 Quaternion::toEulerAngles(const std::string& /*order*/) const
{
  auto result = Vector3::Zero();
  toEulerAnglesToRef(result);
  return result;
}

const Quaternion& Quaternion::toEulerAnglesToRef(Vector3& result,
                                                 const std::string& /*order*/) const
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

const Quaternion& Quaternion::toRotationMatrix(Matrix& result) const
{
  Matrix::FromQuaternionToRef(*this, result);

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
  os << "{\"X\":" << quaternion.x << ",\"Y\":" << quaternion.y << ",\"Z\":" << quaternion.z
     << ",\"W\":" << quaternion.w << "}";
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

void Quaternion::FromRotationMatrixToRef(const Matrix& matrix, Quaternion& result)
{
  const auto& data = matrix.m();
  const auto m11 = data[0], m12 = data[4], m13 = data[8];
  const auto m21 = data[1], m22 = data[5], m23 = data[9];
  const auto m31 = data[2], m32 = data[6], m33 = data[10];
  const auto trace = m11 + m22 + m33;
  float s          = 0.f;

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

float Quaternion::Dot(const Quaternion& left, const Quaternion& right)
{
  return (left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w);
}

bool Quaternion::AreClose(const Quaternion& quat0, const Quaternion& quat1)
{
  const auto dot = Quaternion::Dot(quat0, quat1);
  return dot >= 0.f;
}

Quaternion Quaternion::Zero()
{
  return Quaternion(0.f, 0.f, 0.f, 0.f);
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
  return Quaternion(-q.x, -q.y, -q.z, q.w);
}

Quaternion& Quaternion::InverseToRef(const Quaternion& q, Quaternion& result)
{
  result.set(-q.x, -q.y, -q.z, q.w);
  return result;
}

Quaternion Quaternion::Identity()
{
  return Quaternion(0.f, 0.f, 0.f, 1.f);
}

bool Quaternion::IsIdentity(const Quaternion& quaternion)
{
  return quaternion.x == 0.f && quaternion.y == 0.f && quaternion.z == 0.f && quaternion.w == 1.f;
}

Quaternion Quaternion::RotationAxis(Vector3& axis, float angle)
{
  Quaternion result;
  return Quaternion::RotationAxisToRef(axis, angle, result);
}

Quaternion Quaternion::RotationAxisToRef(Vector3& axis, float angle, Quaternion& result)
{
  const auto _sin = std::sin(angle / 2.f);
  axis.normalize();
  result.w = std::cos(angle / 2.f);
  result.x = axis.x * _sin;
  result.y = axis.y * _sin;
  result.z = axis.z * _sin;
  return result;
}

Quaternion Quaternion::FromArray(const Float32Array& array, unsigned int offset)
{
  return Quaternion(array[offset], array[offset + 1], array[offset + 2], array[offset + 3]);
}

void Quaternion::FromArrayToRef(const Float32Array& array, unsigned int offset, Quaternion& result)
{
  result.x = array[offset];
  result.y = array[offset + 1];
  result.z = array[offset + 2];
  result.w = array[offset + 3];
}

Quaternion Quaternion::FromEulerAngles(float x, float y, float z)
{
  Quaternion q;
  Quaternion::RotationYawPitchRollToRef(y, x, z, q);
  return q;
}

Quaternion Quaternion::FromEulerAnglesToRef(float x, float y, float z, Quaternion& result)
{
  Quaternion::RotationYawPitchRollToRef(y, x, z, result);
  return result;
}

Quaternion Quaternion::FromEulerVector(const Vector3& vec)
{
  Quaternion q;
  Quaternion::RotationYawPitchRollToRef(vec.y, vec.x, vec.z, q);
  return q;
}

Quaternion Quaternion::FromEulerVectorToRef(const Vector3& vec, Quaternion& result)
{
  Quaternion::RotationYawPitchRollToRef(vec.y, vec.x, vec.z, result);
  return result;
}

Quaternion& Quaternion::FromUnitVectorsToRef(const Vector3& vecFrom, const Vector3& vecTo,
                                             Quaternion& result)
{
  const auto r = Vector3::Dot(vecFrom, vecTo) + 1.f;

  if (r < Math::Epsilon) {
    if (std::abs(vecFrom.x) > std::abs(vecFrom.z)) {
      result.set(-vecFrom.y, vecFrom.x, 0.f, 0.f);
    }
    else {
      result.set(0.f, -vecFrom.z, vecFrom.y, 0.f);
    }
  }
  else {
    Vector3::CrossToRef(vecFrom, vecTo, TmpVectors::Vector3Array[0]);
    result.set(TmpVectors::Vector3Array[0].x, //
               TmpVectors::Vector3Array[0].y, //
               TmpVectors::Vector3Array[0].z, //
               r                              //
    );
  }

  return result.normalize();
}

Quaternion Quaternion::RotationYawPitchRoll(float yaw, float pitch, float roll)
{
  Quaternion result;
  Quaternion::RotationYawPitchRollToRef(yaw, pitch, roll, result);
  return result;
}

void Quaternion::RotationYawPitchRollToRef(float yaw, float pitch, float roll, Quaternion& result)
{
  // Produces a quaternion from Euler angles in the z-y-x orientation
  // (Tait-Bryan angles)
  const float halfRoll  = roll * 0.5f;
  const float halfPitch = pitch * 0.5f;
  const float halfYaw   = yaw * 0.5f;

  const float sinRoll  = std::sin(halfRoll);
  const float cosRoll  = std::cos(halfRoll);
  const float sinPitch = std::sin(halfPitch);
  const float cosPitch = std::cos(halfPitch);
  const float sinYaw   = std::sin(halfYaw);
  const float cosYaw   = std::cos(halfYaw);

  result.x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
  result.y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
  result.z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
  result.w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
}

Quaternion Quaternion::RotationAlphaBetaGamma(float alpha, float beta, float gamma)
{
  Quaternion result;
  Quaternion::RotationAlphaBetaGammaToRef(alpha, beta, gamma, result);
  return result;
}

void Quaternion::RotationAlphaBetaGammaToRef(float alpha, float beta, float gamma,
                                             Quaternion& result)
{
  // Produces a quaternion from Euler angles in the z-x-z orientation
  const float halfGammaPlusAlpha  = (gamma + alpha) * 0.5f;
  const float halfGammaMinusAlpha = (gamma - alpha) * 0.5f;
  const float halfBeta            = beta * 0.5f;

  result.x = std::cos(halfGammaMinusAlpha) * std::sin(halfBeta);
  result.y = std::sin(halfGammaMinusAlpha) * std::sin(halfBeta);
  result.z = std::sin(halfGammaPlusAlpha) * std::cos(halfBeta);
  result.w = std::cos(halfGammaPlusAlpha) * std::cos(halfBeta);
}

Quaternion Quaternion::RotationQuaternionFromAxis(Vector3& axis1, Vector3& axis2, Vector3& axis3)
{
  Quaternion quat(0.f, 0.f, 0.f, 0.f);
  Quaternion::RotationQuaternionFromAxisToRef(axis1, axis2, axis3, quat);
  return quat;
}

void Quaternion::RotationQuaternionFromAxisToRef(Vector3& axis1, Vector3& axis2, Vector3& axis3,
                                                 Quaternion& ref)
{
  auto& rotMat = MathTmp::MatrixArray[0];
  Matrix::FromXYZAxesToRef(axis1.normalize(), axis2.normalize(), axis3.normalize(), rotMat);
  Quaternion::FromRotationMatrixToRef(rotMat, ref);
}

Quaternion Quaternion::Slerp(const Quaternion& left, const Quaternion& right, float amount)
{
  Quaternion result = Quaternion::Identity();

  Quaternion::SlerpToRef(left, right, amount, result);

  return result;
}

void Quaternion::SlerpToRef(const Quaternion& left, const Quaternion& right, float amount,
                            Quaternion& result)
{
  float num2, num3;
  float num4
    = (((left.x * right.x) + (left.y * right.y)) + (left.z * right.z)) + (left.w * right.w);
  bool flag = false;

  if (num4 < 0.f) {
    flag = true;
    num4 = -num4;
  }

  if (num4 > 0.999999f) {
    num3 = 1.f - amount;
    num2 = flag ? -amount : amount;
  }
  else {
    const float num5 = std::acos(num4);
    const float num6 = (1.f / std::sin(num5));
    num3             = (std::sin((1.f - amount) * num5)) * num6;
    num2 = flag ? ((-std::sin(amount * num5)) * num6) : ((std::sin(amount * num5)) * num6);
  }

  result.x = (num3 * left.x) + (num2 * right.x);
  result.y = (num3 * left.y) + (num2 * right.y);
  result.z = (num3 * left.z) + (num2 * right.z);
  result.w = (num3 * left.w) + (num2 * right.w);
}

Quaternion Quaternion::Hermite(const Quaternion& value1, const Quaternion& tangent1,
                               const Quaternion& value2, const Quaternion& tangent2, float amount)
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
  const float w
    = (((value1.w * part1) + (value2.w * part2)) + (tangent1.w * part3)) + (tangent2.w * part4);

  return Quaternion(x, y, z, w);
}

} // end of namespace BABYLON
