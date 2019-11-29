#ifndef BABYLON_MATHS_QUATERNION_H
#define BABYLON_MATHS_QUATERNION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Matrix;
class Vector3;

/**
 * @brief Class used to store quaternion data.
 * @see https://en.wikipedia.org/wiki/Quaternion
 * @see http://doc.babylonjs.com/features/position,_rotation,_scaling
 */
class BABYLON_SHARED_EXPORT Quaternion {

public:
  /**
   * @brief Creates a new Quaternion from the given floats.
   * @param x defines the first component (0 by default)
   * @param y defines the second component (0 by default)
   * @param z defines the third component (0 by default)
   * @param w defines the fourth component (1.0 by default)
   */
  Quaternion(float x = 0.f, float y = 0.f, float z = 0.f, float w = 1.f);
  Quaternion(const Quaternion& otherQuaternion);
  Quaternion(Quaternion&& otherQuaternion);
  Quaternion& operator=(const Quaternion& otherQuaternion);
  Quaternion& operator=(Quaternion&& otherQuaternion);
  ~Quaternion(); // = default
  [[nodiscard]] Quaternion copy() const;

  /**
   * @brief Clone the current quaternion.
   * @returns a new quaternion copied from the current one
   */
  [[nodiscard]] std::unique_ptr<Quaternion> clone() const;

  /**
   * @brief Gets a string representation for the current quaternion.
   * @returns a string with the Quaternion coordinates
   */
  [[nodiscard]] std::string toString() const;

  /**
   * @brief Gets the class name of the quaternion.
   * @returns the string "Quaternion"
   */
  [[nodiscard]] const char* getClassName() const;

  /**
   * @brief Gets a hash code for this quaternion.
   * @returns the quaternion hash code
   */
  [[nodiscard]] int getHashCode() const;

  /** Methods **/

  /**
   * @brief Copy the quaternion to an array.
   * @returns a new array populated with 4 elements from the quaternion
   * coordinates
   */
  [[nodiscard]] Float32Array asArray() const;

  /**
   * @brief Check if two quaternions are equals.
   * @param otherQuaternion defines the second operand
   * @return true if the current quaternion and the given one coordinates are
   * strictly equals
   */
  [[nodiscard]] bool equals(const Quaternion& otherQuaternion) const;

  /**
   * @brief Copy a quaternion to the current one.
   * @param other defines the other quaternion
   * @returns the updated current quaternion
   */
  Quaternion& copyFrom(const Quaternion& other);

  /**
   * @brief Updates the current quaternion with the given float coordinates.
   * @param x defines the x coordinate
   * @param y defines the y coordinate
   * @param z defines the z coordinate
   * @param w defines the w coordinate
   * @returns the updated current quaternion
   */
  Quaternion& copyFromFloats(float x, float y, float z, float w);

  /**
   * @brief Updates the current quaternion from the given float coordinates.
   * @param x defines the x coordinate
   * @param y defines the y coordinate
   * @param z defines the z coordinate
   * @param w defines the w coordinate
   * @returns the updated current quaternion
   */
  Quaternion& set(float x, float y, float z, float w);

  /**
   * @brief Adds two quaternions.
   * @param other defines the second operand
   * @returns a new quaternion as the addition result of the given one and the
   * current quaternion
   */
  [[nodiscard]] Quaternion add(const Quaternion& other) const;

  /**
   * @brief Add a quaternion to the current one.
   * @param other defines the quaternion to add
   * @returns the current quaternion
   */
  Quaternion& addInPlace(const Quaternion& other);

  /**
   * @brief Subtract two quaternions.
   * @param other defines the second operand
   * @returns a new quaternion as the subtraction result of the given one from
   * the current one
   */
  [[nodiscard]] Quaternion subtract(const Quaternion& other) const;

  /**
   * @brief Multiplies the current quaternion by a scale factor.
   * @param value defines the scale factor
   * @returns a new quaternion set by multiplying the current quaternion
   * coordinates by the float "scale"
   */
  [[nodiscard]] Quaternion scale(float value) const;

  /**
   * @brief Scale the current quaternion values by a factor and stores the
   * result to a given quaternion.
   * @param scale defines the scale factor
   * @param result defines the Quaternion object where to store the result
   * @returns the unmodified current quaternion
   */
  const Quaternion& scaleToRef(float iscale, Quaternion& result) const;

  /**
   * @brief Multiplies in place the current quaternion by a scale factor.
   * @param value defines the scale factor
   * @returns the current modified quaternion
   */
  Quaternion& scaleInPlace(float value);

  /**
   * @brief Scale the current quaternion values by a factor and add the result
   * to a given quaternion.
   * @param scale defines the scale factor
   * @param result defines the Quaternion object where to store the result
   * @returns the unmodified current quaternion
   */
  const Quaternion& scaleAndAddToRef(float iscale, Quaternion& result) const;

  /**
   * @brief Multiplies two quaternions
   * @param q1 defines the second operand
   * @returns a new quaternion set as the multiplication result of the current
   * one with the given one "q1".
   */
  [[nodiscard]] Quaternion multiply(const Quaternion& q1) const;

  /**
   * @brief Sets the given "result" as the the multiplication result of the
   * current one with the given one "q1".
   * @param q1 defines the second operand
   * @param result defines the target quaternion
   * @returns the current quaternion
   */
  const Quaternion& multiplyToRef(const Quaternion& q1, Quaternion& result) const;

  /**
   * @brief Updates the current quaternion with the multiplication of itself
   * with the given one "q1".
   * @param q1 defines the second operand
   * @returns the currentupdated quaternion
   */
  Quaternion& multiplyInPlace(const Quaternion& q1);

  /**
   * @brief Conjugates (1-q) the current quaternion and stores the result in the
   * given quaternion.
   * @param ref defines the target quaternion
   * @returns the current quaternion
   */
  const Quaternion& conjugateToRef(Quaternion& ref) const;

  /**
   * @brief Conjugates in place (1-q) the current quaternion.
   * @returns the current updated quaternion
   */
  Quaternion& conjugateInPlace();

  /**
   * @brief Conjugates in place (1-q) the current quaternion.
   * @returns a new quaternion
   */
  [[nodiscard]] Quaternion conjugate() const;

  /**
   * @brief Gets length of current quaternion.
   * @returns the quaternion length (float)
   */
  [[nodiscard]] float length() const;

  /**
   * @brief Normalize in place the current quaternion.
   * @returns the current updated quaternion
   */
  Quaternion& normalize();

  /**
   * @brief Returns a new Vector3 set with the Euler angles translated from the
   * current quaternion.
   * @param order is a reserved parameter and is ignore for now
   * @returns a new Vector3 containing the Euler angles
   */
  [[nodiscard]] Vector3 toEulerAngles(const std::string& order = "YZX") const;

  /**
   * @brief Sets the given vector3 "result" with the Euler angles translated
   * from the current quaternion.
   * @param result defines the vector which will be filled with the Euler angles
   * @param order is a reserved parameter and is ignore for now
   * @returns the current unchanged quaternion
   */
  const Quaternion& toEulerAnglesToRef(Vector3& result, const std::string& order = "YZX") const;

  /**
   * @brief Updates the given rotation matrix with the current quaternion
   * values.
   * @param result defines the target matrix
   * @returns the current unchanged quaternion
   */
  const Quaternion& toRotationMatrix(Matrix& result) const;

  /**
   * @brief Updates the current quaternion from the given rotation matrix
   * values.
   * @param matrix defines the source matrix
   * @returns the current updated quaternion
   */
  Quaternion& fromRotationMatrix(const Matrix& matrix);

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion);
  Quaternion operator+(const Quaternion& other) const;
  Quaternion operator-(const Quaternion& other) const;
  Quaternion operator*(float value) const;
  Quaternion operator*(const Quaternion& other) const;
  Quaternion& operator*=(const Quaternion& other);
  bool operator==(const Quaternion& other) const;
  bool operator!=(const Quaternion& other) const;

  /** Statics **/

  /**
   * @brief Creates a new quaternion from a rotation matrix.
   * @param matrix defines the source matrix
   * @returns a new quaternion created from the given rotation matrix values
   */
  static Quaternion FromRotationMatrix(const Matrix& matrix);

  /**
   * @brief Updates the given quaternion with the given rotation matrix values.
   * @param matrix defines the source matrix
   * @param result defines the target quaternion
   */
  static void FromRotationMatrixToRef(const Matrix& matrix, Quaternion& result);

  /**
   * @brief Returns the dot product (float) between the quaternions "left" and
   * "right".
   * @param left defines the left operand
   * @param right defines the right operand
   * @returns the dot product
   */
  static float Dot(const Quaternion& left, const Quaternion& right);

  /**
   * @brief Checks if the two quaternions are close to each other.
   * @param quat0 defines the first quaternion to check
   * @param quat1 defines the second quaternion to check
   * @returns true if the two quaternions are close to each other
   */
  static bool AreClose(const Quaternion& quat0, const Quaternion& quat1);

  /**
   * @brief Creates an empty quaternion.
   * @returns a new quaternion set to (0.0, 0.0, 0.0)
   */
  static Quaternion Zero();

  /**
   * @brief Inverse a given quaternion.
   * @param q defines the source quaternion
   * @returns a new quaternion as the inverted current quaternion
   */
  static Quaternion Inverse(const Quaternion& q);

  /**
   * @brief Inverse a given quaternion.
   * @param q defines the source quaternion
   * @param result the quaternion the result will be stored in
   * @returns the result quaternion
   */
  static Quaternion& InverseToRef(const Quaternion& q, Quaternion& result);

  /**
   * @brief Creates an identity quaternion
   * @returns the identity quaternion
   */
  static Quaternion Identity();

  /**
   * @brief Gets a boolean indicating if the given quaternion is identity.
   * @param quaternion defines the quaternion to check
   * @returns true if the quaternion is identity
   */
  static bool IsIdentity(const Quaternion& quaternion);

  /**
   * @brief Creates a quaternion from a rotation around an axis.
   * @param axis defines the axis to use
   * @param angle defines the angle to use
   * @returns a new quaternion created from the given axis (Vector3) and angle
   * in radians (float)
   */
  static Quaternion RotationAxis(Vector3& axis, float angle);

  /**
   * @brief Creates a rotation around an axis and stores it into the given
   * quaternion.
   * @param axis defines the axis to use
   * @param angle defines the angle to use
   * @param result defines the target quaternion
   * @returns the target quaternion
   */
  static Quaternion RotationAxisToRef(Vector3& axis, float angle, Quaternion& result);

  /**
   * @brief Creates a new quaternion from data stored into an array.
   * @param array defines the data source
   * @param offset defines the offset in the source array where the data starts
   * @returns a new quaternion
   */
  static Quaternion FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Create a quaternion from Euler rotation angles.
   * @param x Pitch
   * @param y Yaw
   * @param z Roll
   * @returns the new Quaternion
   */
  static Quaternion FromEulerAngles(float x, float y, float z);

  /**
   * @brief Updates a quaternion from Euler rotation angles.
   * @param x Pitch
   * @param y Yaw
   * @param z Roll
   * @param result the quaternion to store the result
   * @returns the updated quaternion
   */
  static Quaternion FromEulerAnglesToRef(float x, float y, float z, Quaternion& result);

  /**
   * @brief Create a quaternion from Euler rotation vector.
   * @param vec the Euler vector (x Pitch, y Yaw, z Roll)
   * @returns the new Quaternion
   */
  static Quaternion FromEulerVector(const Vector3& vec);

  /**
   * @brief Updates a quaternion from Euler rotation vector.
   * @param vec the Euler vector (x Pitch, y Yaw, z Roll)
   * @param result the quaternion to store the result
   * @returns the updated quaternion
   */
  static Quaternion FromEulerVectorToRef(const Vector3& vec, Quaternion& result);

  /**
   * @brief Creates a new quaternion from the given Euler float angles (y, x,
   * z).
   * @param yaw defines the rotation around Y axis
   * @param pitch defines the rotation around X axis
   * @param roll defines the rotation around Z axis
   * @returns the new quaternion
   */
  static Quaternion RotationYawPitchRoll(float yaw, float pitch, float roll);

  /**
   * @brief Creates a new rotation from the given Euler float angles (y, x, z)
   * and stores it in the target quaternion.
   * @param yaw defines the rotation around Y axis
   * @param pitch defines the rotation around X axis
   * @param roll defines the rotation around Z axis
   * @param result defines the target quaternion
   */
  static void RotationYawPitchRollToRef(float yaw, float pitch, float roll, Quaternion& result);

  /**
   * @brief Creates a new quaternion from the given Euler float angles expressed
   * in z-x-z orientation.
   * @param alpha defines the rotation around first axis
   * @param beta defines the rotation around second axis
   * @param gamma defines the rotation around third axis
   * @returns the new quaternion
   */
  static Quaternion RotationAlphaBetaGamma(float alpha, float beta, float gamma);

  /**
   * @brief Creates a new quaternion from the given Euler float angles expressed
   * in z-x-z orientation and stores it in the target quaternion.
   * @param alpha defines the rotation around first axis
   * @param beta defines the rotation around second axis
   * @param gamma defines the rotation around third axis
   * @param result defines the target quaternion
   */
  static void RotationAlphaBetaGammaToRef(float alpha, float beta, float gamma, Quaternion& result);

  /**
   * @brief Creates a new quaternion containing the rotation value to reach the
   * target (axis1, axis2, axis3) orientation as a rotated XYZ system (axis1,
   * axis2 and axis3 are normalized during this operation).
   * @param axis1 defines the first axis
   * @param axis2 defines the second axis
   * @param axis3 defines the third axis
   * @returns the new quaternion
   */
  static Quaternion RotationQuaternionFromAxis(Vector3& axis1, Vector3& axis2, Vector3& axis3);

  /**
   * @brief Creates a rotation value to reach the target (axis1, axis2, axis3)
   * orientation as a rotated XYZ system (axis1, axis2 and axis3 are normalized
   * during this operation) and stores it in the target quaternion.
   * @param axis1 defines the first axis
   * @param axis2 defines the second axis
   * @param axis3 defines the third axis
   * @param ref defines the target quaternion
   */
  static void RotationQuaternionFromAxisToRef(Vector3& axis1, Vector3& axis2, Vector3& axis3,
                                              Quaternion& ref);

  /**
   * @brief Interpolates between two quaternions.
   * @param left defines first quaternion
   * @param right defines second quaternion
   * @param amount defines the gradient to use
   * @returns the new interpolated quaternion
   */
  static Quaternion Slerp(const Quaternion& left, const Quaternion& right, float amount);

  /**
   * @brief Interpolates between two quaternions and stores it into a target
   * quaternion.
   * @param left defines first quaternion
   * @param right defines second quaternion
   * @param amount defines the gradient to use
   * @param result defines the target quaternion
   */
  static void SlerpToRef(const Quaternion& left, const Quaternion& right, float amount,
                         Quaternion& result);

  /**
   * @brief Interpolate between two quaternions using Hermite interpolation.
   * @param value1 defines first quaternion
   * @param tangent1 defines the incoming tangent
   * @param value2 defines second quaternion
   * @param tangent2 defines the outgoing tangent
   * @param amount defines the target quaternion
   * @returns the new interpolated quaternion
   */
  static Quaternion Hermite(const Quaternion& value1, const Quaternion& tangent1,
                            const Quaternion& value2, const Quaternion& tangent2, float amount);

public:
  /**
   * Defines the first component (0 by default)
   */
  float x;

  /**
   * Defines the second component (0 by default)
   */
  float y;

  /**
   * Defines the third component (0 by default)
   */
  float z;

  /**
   * Defines the fourth component (1.0 by default)
   */
  float w;

}; // end of class Quaternion

std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion);

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_QUATERNION_H
