#ifndef BABYLON_MATH_VECTOR4_H
#define BABYLON_MATH_VECTOR4_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Vector4 class created for EulerAngle class conversion to Quaternion.
 */
class BABYLON_SHARED_EXPORT Vector4 {

public:
  /**
   * @brief Creates a Vector4 object from the given floats.
   */
  Vector4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f);
  Vector4(const Vector4& otherVector);
  Vector4(Vector4&& otherVector);
  Vector4& operator=(const Vector4& otherVector);
  Vector4& operator=(Vector4&& otherVector);
  ~Vector4();
  Vector4 copy() const;

  /**
   * @brief Returns a new Vector4 copied from the current one.
   */
  std::unique_ptr<Vector4> clone() const;

  /**
   * @brief Returns the string with the Vector4 coordinates.
   */
  std::string toString() const;

  /**
   * @brief Returns the string "Vector4".
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Vector4 hash code.
   */
  size_t getHashCode() const;

  /** Operators **/

  /**
   * @brief Returns a new array populated with 4 elements : the Vector4
   * coordinates.
   */
  Float32Array asArray() const;

  /**
   * @brief Populates the given array from the given index with the Vector4
   * coordinates.
   * @returns the Vector4.
   */
  const Vector4& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Adds the given vector to the current Vector4.
   * @returns the updated Vector4.
   */
  Vector4& addInPlace(const Vector4& otherVector);

  /**
   * @brief Returns a new Vector4 as the result of the addition of the current
   * Vector4 and the given one.
   */
  Vector4 add(const Vector4& otherVector) const;

  /**
   * @brief Updates the given vector "result" with the result of the addition of
   * the current Vector4 and the given one.
   * @returns the current Vector4.
   */
  const Vector4& addToRef(const Vector4& otherVector, Vector4& result) const;

  /**
   * @brief Subtract in place the given vector from the current Vector4.
   * @returns the updated Vector4.
   */
  Vector4& subtractInPlace(const Vector4& otherVector);

  /**
   * @brief Returns a new Vector4 with the result of the subtraction of the
   * given vector from the current Vector4.
   */
  Vector4 subtract(const Vector4& otherVector) const;

  /**
   * @brief Sets the given vector "result" with the result of the subtraction of
   * the given vector from the current Vector4.
   * @returns the current Vector4.
   */
  const Vector4& subtractToRef(const Vector4& otherVector,
                               Vector4& result) const;

  /**
   * @brief Returns a new Vector4 set with the result of the subtraction of the
   * given floats from the current Vector4 coordinates.
   */
  Vector4 subtractFromFloats(float ix, float iy, float iz, float iw) const;

  /**
   * @brief Sets the given vector "result" set with the result of the
   * subtraction of the given floats from the current Vector4 coordinates.
   * @returns the current Vector4.
   */
  const Vector4& subtractFromFloatsToRef(float ix, float iy, float iz, float iw,
                                         Vector4& result) const;

  /**
   * @brief Returns a new Vector4 set with the current Vector4 negated
   * coordinates.
   */
  Vector4 negate() const;

  /**
   * @brief Multiplies the current Vector4 coordinates by scale (float).
   * @returns The updated Vector4.
   */
  Vector4& scaleInPlace(float iscale);

  /**
   * @brief Returns a new Vector4 set with the current Vector4 coordinates
   * multiplied by scale (float).
   */
  Vector4 scale(float iscale) const;

  /**
   * @brief Sets the given vector "result" with the current Vector4 coordinates
   * multiplied by scale (float).
   * @returns the current Vector4.
   */
  const Vector4& scaleToRef(float iscale, Vector4& result) const;

  /**
   * @brief Scale the current Vector4 values by a factor and add the result to a
   * given Vector4.
   * @param scale defines the scale factor
   * @param result defines the Vector4 object where to store the result
   * @returns the unmodified current Vector4
   */
  const Vector4& scaleAndAddToRef(float iscale, Vector4& result) const;

  /**
   * @brief Boolean : True if the current Vector4 coordinates are stricly equal
   * to the given ones.
   */
  bool equals(const Vector4& otherVector) const;

  /**
   * @brief Boolean : True if the current Vector4 coordinates are each beneath
   * the distance "epsilon" from the given vector ones.
   */
  bool equalsWithEpsilon(const Vector4& otherVector,
                         float epsilon = Math::Epsilon) const;

  /**
   * @brief Boolean : True if the given floats are strictly equal to the current
   * Vector4 coordinates.
   */
  bool equalsToFloats(float x, float y, float z, float w) const;

  /**
   * @brief Multiplies in place the current Vector4 by the given one.
   * @returns the updated Vector4.
   */
  Vector4& multiplyInPlace(const Vector4& otherVector);

  /**
   * @brief Returns a new Vector4 set with the multiplication result of the
   * current Vector4 and the given one.
   */
  Vector4 multiply(const Vector4& otherVector) const;

  /**
   * @brief Updates the given vector "result" with the multiplication result of
   * the current Vector4 and the given one.
   * @returns the current Vector4.
   */
  const Vector4& multiplyToRef(const Vector4& otherVector,
                               Vector4& result) const;

  /**
   * @brief Returns a new Vector4 set with the multiplication result of the
   * given floats and the current Vector4 coordinates.
   */
  Vector4 multiplyByFloats(float x, float y, float z, float w) const;

  /**
   * @brief Returns a new Vector4 set with the division result of the current
   * Vector4 by the given one.
   */
  Vector4 divide(const Vector4& otherVector) const;

  /**
   *@brief Updates the given vector "result" with the division result of the
   *current Vector4 by the given one.
   * @returns the current Vector4.
   */
  const Vector4& divideToRef(const Vector4& otherVector, Vector4& result) const;

  /**
   * @brief Divides the current Vector3 coordinates by the given ones.
   * @returns the updated Vector3.
   */
  Vector4& divideInPlace(const Vector4& otherVector);

  /**
   * @brief Updates the Vector4 coordinates with the minimum values between its
   * own and the given vector ones.
   * @param other defines the second operand
   * @returns the current updated Vector4
   */
  Vector4& minimizeInPlace(const Vector4& other);

  /**
   * @brief Updates the Vector4 coordinates with the maximum values between its
   * own and the given vector ones.
   * @param other defines the second operand
   * @returns the current updated Vector4
   */
  Vector4& maximizeInPlace(const Vector4& other);

  /**
   * @brief Gets a new Vector4 from current Vector4 floored values.
   * @returns a new Vector4
   */
  Vector4 floor() const;

  /**
   * @brief Gets a new Vector4 from current Vector3 floored values.
   * @returns a new Vector4
   */
  Vector4 fract() const;

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const Vector4& vector);
  Vector4 operator+(const Vector4& otherVector) const;
  Vector4& operator+=(const Vector4& otherVector);
  Vector4 operator-(const Vector4& otherVector) const;
  Vector4& operator-=(const Vector4& otherVector);
  Vector4 operator-() const;
  Vector4 operator*(float scaleVal) const;
  Vector4& operator*=(float scaleVal);
  Vector4 operator*(const Vector4& otherVector) const;
  Vector4& operator*=(const Vector4& otherVector);
  Vector4 operator/(const Vector4& otherVector) const;
  bool operator==(const Vector4& otherVector) const;
  bool operator!=(const Vector4& otherVector) const;

  /** Properties **/

  /**
   * @brief Returns the Vector4 length (float).
   */
  float length() const;

  /**
   * @brief Returns the Vector4 squared length (float).
   */
  float lengthSquared() const;

  /** Methods **/

  /**
   * @brief Normalizes in place the Vector4.
   * @returns The updated Vector4.
   */
  Vector4& normalize();

  /**
   * @brief Returns a new Vector3 from the Vector4 (x, y, z) coordinates.
   */
  Vector3 toVector3() const;

  /**
   * @brief Updates the current Vector4 with the given one coordinates.
   * @returns the updated Vector4.
   */
  Vector4& copyFrom(const Vector4& source);

  /**
   * @brief Updates the current Vector4 coordinates with the given floats.
   * @returns the updated Vector4.
   */
  Vector4& copyFromFloats(float x, float y, float z, float w);

  /**
   * @brief Updates the current Vector4 coordinates with the given floats.
   * @returns the updated Vector4.
   */
  Vector4& set(float x, float y, float z, float w);

  /** Statics **/

  /**
   * @brief Returns a new Vector4 set from the starting index of the given
   * array.
   */
  static Vector4 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Updates the given vector "result" from the starting index of the
   * given Float32Array.
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector4& result);

  /**
   * @brief Updates the given vector "result" coordinates from the given floats.
   */
  static void FromFloatsToRef(float x, float y, float z, float w,
                              Vector4& result);

  /**
   * @brief Returns a new Vector4 set to (0.0, 0.0, 0.0, 0.0)
   */
  static Vector4 Zero();

  /**
   * @brief Returns a new Vector4 set to (1.0, 1.0, 1.0, 1.0)
   */
  static Vector4 One();

  /**
   * @brief Returns a new normalized Vector4 from the given one.
   */
  static Vector4 Normalize(const Vector4& vector);

  /**
   * @brief Updates the given vector "result" from the normalization of the
   * given one.
   */
  static void NormalizeToRef(const Vector4& vector, Vector4& result);

  static Vector4 Minimize(const Vector4& left, const Vector4& right);
  static Vector4 Maximize(const Vector4& left, const Vector4& right);

  /**
   * @brief Returns the distance (float) between the vectors "value1" and
   * "value2".
   */
  static float Distance(const Vector4& value1, const Vector4& value2);

  /**
   * @brief Returns the squared distance (float) between the vectors "value1"
   * and "value2".
   */
  static float DistanceSquared(const Vector4& value1, const Vector4& value2);

  /**
   * @brief Returns a new Vector4 located at the center between the vectors
   * "value1" and "value2".
   */
  static Vector4 Center(const Vector4& value1, const Vector4& value2);

  /**
   * @brief Returns a new Vector4 set with the result of the normal
   * transformation by the given matrix of the given vector. This methods
   * computes transformed normalized direction vectors only.
   */
  static Vector4 TransformNormal(const Vector4& vector,
                                 const Matrix& transformation);

  /**
   * @brief Sets the given vector "result" with the result of the normal
   * transformation by the given matrix of the given vector. This methods
   * computes transformed normalized direction vectors only.
   */
  static void TransformNormalToRef(const Vector4& vector,
                                   const Matrix& transformation,
                                   Vector4& result);

  /**
   * @brief Sets the given vector "result" with the result of the normal
   * transformation by the given matrix of the given floats (x, y, z, w). This
   * methods computes transformed normalized direction vectors only.
   */
  static void TransformNormalFromFloatsToRef(float x, float y, float z, float w,
                                             const Matrix& transformation,
                                             Vector4& result);

public:
  float x;
  float y;
  float z;
  float w;

}; // end of class Vector4

std::ostream& operator<<(std::ostream& os, const Vector4& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR4_H
