#ifndef BABYLON_MATH_QUATERNION_H
#define BABYLON_MATH_QUATERNION_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Quaternion.
 */
class BABYLON_SHARED_EXPORT Quaternion {

public:
  /**
   * @brief Creates a new Quaternion from the passed floats.
   */
  Quaternion(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f);
  Quaternion(const Quaternion& otherQuaternion);
  Quaternion(Quaternion&& otherQuaternion);
  Quaternion& operator=(const Quaternion& otherQuaternion);
  Quaternion& operator=(Quaternion&& otherQuaternion);
  ~Quaternion();
  Quaternion copy() const;

  /**
   * @brief Returns a new Quaternion copied from the current one.
   */
  std::unique_ptr<Quaternion> clone() const;

  /**
   * @brief Returns a string with the Quaternion coordinates.
   */
  std::string toString() const;

  /**
   * @brief Returns the string "Quaternion".
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Quaternion hash code.
   */
  int getHashCode() const;

  /** Methods **/

  /**
   * @brief Returns a new array populated with 4 elements : the Quaternion
   * coordinates.
   */
  Float32Array asArray() const;

  /**
   * @brief Returns if the current Quaterion and the passed one coordinates are
   * strictly equal.
   */
  bool equals(const Quaternion& otherQuaternion) const;

  /**
   * @brief Updates the current Quaternion from the passed one coordinates.
   * @returns The updated Quaterion.
   */
  Quaternion& copyFrom(const Quaternion& other);

  /**
   * @brief Updates the current Quaternion from the passed float coordinates.
   * @returns The updated Quaterion.
   */
  Quaternion& copyFromFloats(float x, float y, float z, float w);

  /**
   * @brief Updates the current Quaternion from the passed float coordinates.
   * @returns The updated Quaterion.
   */
  Quaternion& set(float x, float y, float z, float w);

  /**
   * @brief Returns a new Quaternion as the addition result of the passed one
   * and the current Quaternion.
   */
  Quaternion add(const Quaternion& other) const;

  /**
   * @brief Returns a new Quaternion as the subtraction result of the passed one
   * from the current Quaternion.
   */
  Quaternion subtract(const Quaternion& other) const;

  /**
   * @brief Returns a new Quaternion set by multiplying the current Quaterion
   * coordinates by the float "scale".
   */
  Quaternion scale(float value) const;

  /**
   * @brief Returns a new Quaternion set as the quaternion mulplication result
   * of the current one with the passed one "q1".
   */
  Quaternion multiply(const Quaternion& q1) const;

  /**
   * @brief Sets the passed "result" as the quaternion mulplication result of
   * the current one with the passed one "q1".
   * @returns The current Quaternion.
   */
  const Quaternion& multiplyToRef(const Quaternion& q1,
                                  Quaternion& result) const;

  /**
   * @brief Updates the current Quaternion with the quaternion mulplication
   * result of itself with the passed one "q1".
   * @returns The updated Quaternion.
   */
  Quaternion& multiplyInPlace(const Quaternion& q1);

  /**
   * @brief Sets the passed "ref" with the conjugation of the current
   * Quaternion.
   * @returns The current Quaternion.
   */
  const Quaternion& conjugateToRef(Quaternion& ref) const;

  /**
   * @brief Conjugates in place the current Quaternion.
   * @returns The updated Quaternion.
   */
  Quaternion& conjugateInPlace();

  /**
   * @brief Returns a new Quaternion as the conjugate of the current Quaternion.
   */
  Quaternion conjugate() const;

  /**
   * @brief Returns the Quaternion length (float).
   */
  float length() const;

  /**
   * @brief Normalize in place the current Quaternion.
   * @returns The updated Quaternion.
   */
  Quaternion& normalize();

  /**
   * @brief Returns a new Vector3 set with the Euler angles translated from the
   * current Quaternion.
   */
  Vector3 toEulerAngles() const;

  /**
   * @brief Sets the passed vector3 "result" with the Euler angles translated
   * from the current Quaternion.
   * @returns The current Quaternion.
   */
  const Quaternion& toEulerAnglesToRef(Vector3& result,
                                       const std::string& order = "YZX") const;

  /**
   * @brief Updates the passed rotation matrix with the current Quaternion
   * values.
   * @returns The current Quaternion.
   */
  const Quaternion& toRotationMatrix(Matrix& result) const;

  /**
   * @brief Updates the current Quaternion from the passed rotation matrix
   * values.
   * @returns The updated Quaternion.
   */
  Quaternion& fromRotationMatrix(const Matrix& matrix);

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os,
                                  const Quaternion& quaternion);
  Quaternion operator+(const Quaternion& other) const;
  Quaternion operator-(const Quaternion& other) const;
  Quaternion operator*(float value) const;
  Quaternion operator*(const Quaternion& other) const;
  Quaternion& operator*=(const Quaternion& other);
  bool operator==(const Quaternion& other) const;
  bool operator!=(const Quaternion& other) const;

  /** Statics **/

  /**
   * @brief Returns a new Quaternion set from the passed rotation matrix values.
   */
  static Quaternion FromRotationMatrix(const Matrix& matrix);

  /**
   * @brief Updates the passed quaternion "result" with the passed rotation
   * matrix values.
   */
  static void FromRotationMatrixToRef(const Matrix& matrix, Quaternion& result);

  /**
   * @brief Returns a new Quaternion set to (0.0, 0.0, 0.0).
   */
  static Quaternion Zero();

  /**
   * @brief Returns a new Quaternion as the inverted current Quaternion.
   */
  static Quaternion Inverse(const Quaternion& q);

  /**
   * @brief Returns the identity Quaternion.
   */
  static Quaternion Identity();

  /**
   * @brief Returns if the quaternion is the identity Quaternion.
   */
  static bool IsIdentity(const Quaternion& quaternion);

  /**
   * @brief Returns a new Quaternion set from the passed axis (Vector3) and
   * angle in radians (float).
   */
  static Quaternion RotationAxis(Vector3& axis, float angle);

  /**
   * @brief Sets the passed quaternion "result" from the passed axis (Vector3)
   * and angle in radians (float).
   */
  static Quaternion RotationAxisToRef(Vector3& axis, float angle,
                                      Quaternion& result);

  /**
   * @brief Retuns a new Quaternion set from the starting index of the passed
   * array.
   */
  static Quaternion FromArray(const Float32Array& array,
                              unsigned int offset = 0);

  /**
   * @brief Returns a new Quaternion set from the passed Euler float angles (y,
   * x, z).
   */
  static Quaternion RotationYawPitchRoll(float yaw, float pitch, float roll);

  /**
   * @brief Sets the passed quaternion "result" from the passed float Euler
   * angles (y, x, z).
   */
  static void RotationYawPitchRollToRef(float yaw, float pitch, float roll,
                                        Quaternion& result);

  /**
   * @brief Returns a new Quaternion from the passed float Euler angles
   * expressed in z-x-z orientation
   */
  static Quaternion RotationAlphaBetaGamma(float alpha, float beta,
                                           float gamma);

  /**
   * @brief Sets the passed quaternion "result" from the passed float Euler
   * angles expressed in z-x-z orientation
   */
  static void RotationAlphaBetaGammaToRef(float alpha, float beta, float gamma,
                                          Quaternion& result);

  static Quaternion RotationQuaternionFromAxis(Vector3& axis1, Vector3& axis2,
                                               Vector3& axis3);

  /**
   * @brief Sets the passed quaternion "ref" with the quaternion rotation value
   * to reach the target (axis1, axis2, axis3) orientation as a rotated XYZ
   * system.
   * cf to Vector3.RotationFromAxis() documentation.
   * Note : axis1, axis2 and axis3 are normalized during this operation.
   */
  static void RotationQuaternionFromAxisToRef(Vector3& axis1, Vector3& axis2,
                                              Vector3& axis3, Quaternion& ref);

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
