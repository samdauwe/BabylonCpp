#ifndef BABYLON_MATH_QUATERNION_H
#define BABYLON_MATH_QUATERNION_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Quaternion.
 */
class BABYLON_SHARED_EXPORT Quaternion {

public:
  Quaternion(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f);
  Quaternion(const Quaternion& otherQuaternion);
  Quaternion(Quaternion&& otherQuaternion);
  Quaternion& operator=(const Quaternion& otherQuaternion);
  Quaternion& operator=(Quaternion&& otherQuaternion);
  ~Quaternion();
  Quaternion copy() const;
  std::unique_ptr<Quaternion> clone() const;
  friend std::ostream& operator<<(std::ostream& os,
                                  const Quaternion& quaternion);
  const char* getClassName() const;
  int getHashCode() const;

  /** Methods **/
  Float32Array asArray() const;
  bool equals(const Quaternion& otherQuaternion) const;
  Quaternion& copyFrom(const Quaternion& other);
  Quaternion& copyFromFloats(float x, float y, float z, float w);
  Quaternion add(const Quaternion& other) const;
  Quaternion subtract(const Quaternion& other) const;
  Quaternion scale(float value) const;
  Quaternion multiply(const Quaternion& q1);
  Quaternion& multiplyToRef(const Quaternion& q1, Quaternion& result);
  Quaternion& multiplyInPlace(const Quaternion& q1);
  Quaternion& conjugateToRef(Quaternion& ref);
  Quaternion& conjugateInPlace();
  Quaternion conjugate() const;
  float length() const;
  Quaternion& normalize();
  Vector3 toEulerAngles();
  Quaternion& toEulerAnglesToRef(Vector3& result,
                                 const std::string& order = "YZX");
  Quaternion& toRotationMatrix(Matrix& result);
  Quaternion& fromRotationMatrix(const Matrix& matrix);

  /** Operator overloading **/
  Quaternion operator+(const Quaternion& other) const;
  Quaternion operator-(const Quaternion& other) const;
  Quaternion operator*(float value) const;
  Quaternion operator*(const Quaternion& other);
  Quaternion& operator*=(const Quaternion& other);
  bool operator==(const Quaternion& other) const;
  bool operator!=(const Quaternion& other) const;

  /** Statics **/
  static Quaternion FromRotationMatrix(const Matrix& matrix);
  static void FromRotationMatrixToRef(const Matrix& matrix, Quaternion& result);
  static Quaternion Inverse(const Quaternion& q);
  static Quaternion Identity();
  static Quaternion RotationAxis(Vector3& axis, float angle);
  static Quaternion RotationAxisToRef(Vector3& axis, float angle,
                                      Quaternion& result);
  static Quaternion FromArray(const Float32Array& array,
                              unsigned int offset = 0);
  static Quaternion RotationYawPitchRoll(float yaw, float pitch, float roll);
  static void RotationYawPitchRollToRef(float yaw, float pitch, float roll,
                                        Quaternion& result);
  static Quaternion RotationAlphaBetaGamma(float alpha, float beta,
                                           float gamma);
  static void RotationAlphaBetaGammaToRef(float alpha, float beta, float gamma,
                                          Quaternion& result);
  static Quaternion Slerp(const Quaternion& left, const Quaternion& right,
                          float amount);
  static void SlerpToRef(const Quaternion& left, const Quaternion& right,
                         float amount, Quaternion& result);

public:
  float x;
  float y;
  float z;
  float w;

}; // end of class Quaternion

std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_QUATERNION_H
