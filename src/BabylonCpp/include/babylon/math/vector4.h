#ifndef BABYLON_MATH_VECTOR4_H
#define BABYLON_MATH_VECTOR4_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Vector4 class created for EulerAngle class conversion to Quaternion
 */
class BABYLON_SHARED_EXPORT Vector4 {

public:
  /**
   * @brief Creates a Vector4 object from the given floats.
   * @param x x value of the vector
   * @param y y value of the vector
   * @param z z value of the vector
   * @param w w value of the vector
   */
  Vector4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f);
  Vector4(const Vector4& otherVector);
  Vector4(Vector4&& otherVector);
  Vector4& operator=(const Vector4& otherVector);
  Vector4& operator=(Vector4&& otherVector);
  ~Vector4(); // = default
  [[nodiscard]] Vector4 copy() const;

  /**
   * @brief Returns a new Vector4 copied from the current one.
   * @returns the new cloned vector
   */
  [[nodiscard]] std::unique_ptr<Vector4> clone() const;

  /**
   * @brief Returns the string with the Vector4 coordinates.
   * @returns a string containing all the vector values
   */
  [[nodiscard]] std::string toString() const;

  /**
   * @brief Returns the string "Vector4".
   * @returns "Vector4"
   */
  [[nodiscard]] const char* getClassName() const;

  /**
   * @brief Returns the Vector4 hash code.
   * @returns a unique hash code
   */
  [[nodiscard]] size_t getHashCode() const;

  /** Operators **/

  /**
   * @brief Returns a new array populated with 4 elements : the Vector4
   * coordinates.
   * @returns the resulting array
   */
  [[nodiscard]] Float32Array asArray() const;

  /**
   * @brief Populates the given array from the given index with the Vector4
   * coordinates.
   * @param array array to populate
   * @param index index of the array to start at (default: 0)
   * @returns the Vector4.
   */
  const Vector4& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Adds the given vector to the current Vector4.
   * @param otherVector the vector to add
   * @returns the updated Vector4.
   */
  Vector4& addInPlace(const Vector4& otherVector);

  /**
   * @brief Returns a new Vector4 as the result of the addition of the current
   * Vector4 and the given one.
   * @param otherVector the vector to add
   * @returns the resulting vector
   */
  [[nodiscard]] Vector4 add(const Vector4& otherVector) const;

  /**
   * @brief Updates the given vector "result" with the result of the addition of
   * the current Vector4 and the given one.
   * @param otherVector the vector to add
   * @param result the vector to store the result
   * @returns the current Vector4.
   */
  const Vector4& addToRef(const Vector4& otherVector, Vector4& result) const;

  /**
   * @brief Subtract in place the given vector from the current Vector4.
   * @param otherVector the vector to subtract
   * @returns the updated Vector4.
   */
  Vector4& subtractInPlace(const Vector4& otherVector);

  /**
   * @brief Returns a new Vector4 with the result of the subtraction of the
   * given vector from the current Vector4.
   * @param otherVector the vector to add
   * @returns the new vector with the result
   */
  [[nodiscard]] Vector4 subtract(const Vector4& otherVector) const;

  /**
   * @brief Sets the given vector "result" with the result of the subtraction of
   * the given vector from the current Vector4.
   * @param otherVector the vector to subtract
   * @param result the vector to store the result
   * @returns the current Vector4.
   */
  const Vector4& subtractToRef(const Vector4& otherVector, Vector4& result) const;

  /**
   * @brief Returns a new Vector4 set with the result of the subtraction of the
   * given floats from the current Vector4 coordinates.
   * @param x value to subtract
   * @param y value to subtract
   * @param z value to subtract
   * @param w value to subtract
   * @returns new vector containing the result
   */
  [[nodiscard]] Vector4 subtractFromFloats(float ix, float iy, float iz, float iw) const;

  /**
   * @brief Sets the given vector "result" set with the result of the
   * subtraction of the given floats from the current Vector4 coordinates.
   * @param x value to subtract
   * @param y value to subtract
   * @param z value to subtract
   * @param w value to subtract
   * @param result the vector to store the result in
   * @returns the current Vector4.
   */
  const Vector4& subtractFromFloatsToRef(float ix, float iy, float iz, float iw,
                                         Vector4& result) const;

  /**
   * @brief Returns a new Vector4 set with the current Vector4 negated
   * coordinates.
   * @returns a new vector with the negated values
   */
  [[nodiscard]] Vector4 negate() const;

  /**
   * @brief Multiplies the current Vector4 coordinates by scale (float).
   * @param scale the number to scale with
   * @returns the updated Vector4.
   */
  Vector4& scaleInPlace(float iscale);

  /**
   * @brief Returns a new Vector4 set with the current Vector4 coordinates
   * multiplied by scale (float).
   * @param scale the number to scale with
   * @returns a new vector with the result
   */
  [[nodiscard]] Vector4 scale(float iscale) const;

  /**
   * @brief Sets the given vector "result" with the current Vector4 coordinates
   * multiplied by scale (float).
   * @param scale the number to scale with
   * @param result a vector to store the result in
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
   * @param otherVector the vector to compare against
   * @returns true if they are equal
   */
  [[nodiscard]] bool equals(const Vector4& otherVector) const;

  /**
   * @brief Boolean : True if the current Vector4 coordinates are each beneath
   * the distance "epsilon" from the given vector ones.
   * @param otherVector vector to compare against
   * @param epsilon (Default: very small number)
   * @returns true if they are equal
   */
  [[nodiscard]] bool equalsWithEpsilon(const Vector4& otherVector,
                                       float epsilon = Math::Epsilon) const;

  /**
   * @brief Boolean : True if the given floats are strictly equal to the current
   * Vector4 coordinates.
   * @param x x value to compare against
   * @param y y value to compare against
   * @param z z value to compare against
   * @param w w value to compare against
   * @returns true if equal
   */
  [[nodiscard]] bool equalsToFloats(float x, float y, float z, float w) const;

  /**
   * @brief Multiplies in place the current Vector4 by the given one.
   * @returns the updated Vector4.
   * @param otherVector vector to multiple with
   * @returns the updated Vector4.
   */
  Vector4& multiplyInPlace(const Vector4& otherVector);

  /**
   * @brief Returns a new Vector4 set with the multiplication result of the
   * current Vector4 and the given one.
   * @param otherVector vector to multiple with
   * @returns resulting new vector
   */
  [[nodiscard]] Vector4 multiply(const Vector4& otherVector) const;

  /**
   * @brief Updates the given vector "result" with the multiplication result of
   * the current Vector4 and the given one.
   * @returns the current Vector4.
   * @param otherVector vector to multiple with
   * @param result vector to store the result
   * @returns the current Vector4.
   */
  const Vector4& multiplyToRef(const Vector4& otherVector, Vector4& result) const;

  /**
   * @brief Returns a new Vector4 set with the multiplication result of the
   * given floats and the current Vector4 coordinates.
   * @param x x value multiply with
   * @param y y value multiply with
   * @param z z value multiply with
   * @param w w value multiply with
   * @returns resulting new vector
   */
  [[nodiscard]] Vector4 multiplyByFloats(float x, float y, float z, float w) const;

  /**
   * @brief Returns a new Vector4 set with the division result of the current
   * Vector4 by the given one.
   * @param otherVector vector to devide with
   * @returns resulting new vector
   */
  [[nodiscard]] Vector4 divide(const Vector4& otherVector) const;

  /**
   *@brief Updates the given vector "result" with the division result of the
   *current Vector4 by the given one.
   * @param otherVector vector to devide with
   * @param result vector to store the result
   * @returns the current Vector4.
   */
  const Vector4& divideToRef(const Vector4& otherVector, Vector4& result) const;

  /**
   * @brief Divides the current Vector3 coordinates by the given ones.
   * @param otherVector vector to devide with
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
  [[nodiscard]] Vector4 floor() const;

  /**
   * @brief Gets a new Vector4 from current Vector3 floored values.
   * @returns a new Vector4
   */
  [[nodiscard]] Vector4 fract() const;

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
   * @returns the length
   */
  [[nodiscard]] float length() const;

  /**
   * @brief Returns the Vector4 squared length (float).
   * @returns the length squared
   */
  [[nodiscard]] float lengthSquared() const;

  /** Methods **/

  /**
   * @brief Normalizes in place the Vector4.
   * @returns the updated Vector4.
   */
  Vector4& normalize();

  /**
   * @brief Returns a new Vector3 from the Vector4 (x, y, z) coordinates.
   * @returns this converted to a new vector3
   */
  [[nodiscard]] Vector3 toVector3() const;

  /**
   * @brief Updates the current Vector4 with the given one coordinates.
   * @param source the source vector to copy from
   * @returns the updated Vector4.
   */
  Vector4& copyFrom(const Vector4& source);

  /**
   * @brief Updates the current Vector4 coordinates with the given floats.
   * @param x float to copy from
   * @param y float to copy from
   * @param z float to copy from
   * @param w float to copy from
   * @returns the updated Vector4.
   */
  Vector4& copyFromFloats(float x, float y, float z, float w);

  /**
   * @brief Updates the current Vector4 coordinates with the given floats.
   * @param x float to set from
   * @param y float to set from
   * @param z float to set from
   * @param w float to set from
   * @returns the updated Vector4.
   */
  Vector4& set(float x, float y, float z, float w);

  /**
   * @brief Copies the given float to the current Vector3 coordinates.
   * @param v defines the x, y, z and w coordinates of the operand
   * @returns the current updated Vector3
   */
  Vector4& setAll(float v);

  /** Statics **/

  /**
   * @brief Returns a new Vector4 set from the starting index of the given
   * array.
   * @param array the array to pull values from
   * @param offset the offset into the array to start at
   * @returns the new vector
   */
  static Vector4 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Updates the given vector "result" from the starting index of the
   * given array.
   * @param array the array to pull values from
   * @param offset the offset into the array to start at
   * @param result the vector to store the result in
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset, Vector4& result);

  /**
   * @brief Updates the given vector "result" coordinates from the given floats.
   * @param x float to set from
   * @param y float to set from
   * @param z float to set from
   * @param w float to set from
   * @param result the vector to the floats in
   */
  static void FromFloatsToRef(float x, float y, float z, float w, Vector4& result);

  /**
   * @brief Returns a new Vector4 set to (0.0, 0.0, 0.0, 0.0)
   * @returns the new vector
   */
  static Vector4 Zero();

  /**
   * @brief Returns a new Vector4 set to (1.0, 1.0, 1.0, 1.0)
   * @returns the new vector
   */
  static Vector4 One();

  /**
   * @brief Returns a new normalized Vector4 from the given one.
   * @param vector the vector to normalize
   * @returns the vector
   */
  static Vector4 Normalize(const Vector4& vector);

  /**
   * @brief pdates the given vector "result" from the normalization of the given
   * one.
   * @param vector the vector to normalize
   * @param result the vector to store the result in
   */
  static void NormalizeToRef(const Vector4& vector, Vector4& result);

  /**
   * @brief Returns a vector with the minimum values from the left and right
   * vectors
   * @param left left vector to minimize
   * @param right right vector to minimize
   * @returns a new vector with the minimum of the left and right vector values
   */
  static Vector4 Minimize(const Vector4& left, const Vector4& right);

  /**
   * @brief Returns a vector with the maximum values from the left and right
   * vectors
   * @param left left vector to maximize
   * @param right right vector to maximize
   * @returns a new vector with the maximum of the left and right vector values
   */
  static Vector4 Maximize(const Vector4& left, const Vector4& right);

  /**
   * @brief Returns the distance (float) between the vectors "value1" and
   * "value2".
   * @param value1 value to calulate the distance between
   * @param value2 value to calulate the distance between
   * @return the distance between the two vectors
   */
  static float Distance(const Vector4& value1, const Vector4& value2);

  /**
   * @brief Returns the squared distance (float) between the vectors "value1"
   * and "value2".
   * @param value1 value to calulate the distance between
   * @param value2 value to calulate the distance between
   * @return the distance between the two vectors squared
   */
  static float DistanceSquared(const Vector4& value1, const Vector4& value2);

  /**
   * @brief Returns a new Vector4 located at the center between the vectors
   * "value1" and "value2".
   * @param value1 value to calulate the center between
   * @param value2 value to calulate the center between
   * @return the center between the two vectors
   */
  static Vector4 Center(const Vector4& value1, const Vector4& value2);

  /**
   * @brief Returns a new Vector4 set with the result of the normal
   * transformation by the given matrix of the given vector. This methods
   * computes transformed normalized direction vectors only.
   * @param vector the vector to transform
   * @param transformation the transformation matrix to apply
   * @returns the new vector
   */
  static Vector4 TransformNormal(const Vector4& vector, const Matrix& transformation);

  /**
   * @brief Sets the given vector "result" with the result of the normal
   * transformation by the given matrix of the given vector. This methods
   * computes transformed normalized direction vectors only.
   * @param vector the vector to transform
   * @param transformation the transformation matrix to apply
   * @param result the vector to store the result in
   */
  static void TransformNormalToRef(const Vector4& vector, const Matrix& transformation,
                                   Vector4& result);

  /**
   * @brief Sets the given vector "result" with the result of the normal
   * transformation by the given matrix of the given floats (x, y, z, w). This
   * methods computes transformed normalized direction vectors only.
   * @param x value to transform
   * @param y value to transform
   * @param z value to transform
   * @param w value to transform
   * @param transformation the transformation matrix to apply
   * @param result the vector to store the results in
   */
  static void TransformNormalFromFloatsToRef(float x, float y, float z, float w,
                                             const Matrix& transformation, Vector4& result);

  /**
   * @brief Creates a new Vector4 from a Vector3.
   * @param source defines the source data
   * @param w defines the 4th component (default is 0)
   * @returns a new Vector4
   */
  static Vector4 FromVector3(const Vector3& source, float w = 0.f);

public:
  /**
   * x value of the vector
   */
  float x;

  /**
   * y value of the vector
   */
  float y;

  /**
   * z value of the vector
   */
  float z;

  /**
   * w value of the vector
   */
  float w;

}; // end of class Vector4

std::ostream& operator<<(std::ostream& os, const Vector4& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR4_H
