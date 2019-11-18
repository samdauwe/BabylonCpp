#ifndef BABYLON_MATH_VECTOR3_H
#define BABYLON_MATH_VECTOR3_H

#include <array>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_constants.h>

namespace BABYLON {

class Matrix;
class Quaternion;
class Viewport;

/**
 * @brief Classed used to store (x,y,z) vector representation
 * A Vector3 is the main object used in 3D geometry
 * It can represent etiher the coordinates of a point the space, either a
 * direction
 * Reminder: js uses a left handed forward facing system
 */
class BABYLON_SHARED_EXPORT Vector3 {

private:
  static const Vector3 _UpReadOnly;

public:
  /**
   * @brief Creates a new Vector3 object from the given x, y, z (floats)
   * coordinates.
   * @param x defines the first coordinates (on X axis)
   * @param y defines the second coordinates (on Y axis)
   * @param z defines the third coordinates (on Z axis)
   */
  Vector3();
  Vector3(float x, float y, float z);
  Vector3(const Vector3& otherVector);
  Vector3(Vector3&& otherVector);
  ~Vector3(); // = default
  Vector3& operator=(const Vector3& otherVector);
  Vector3& operator=(Vector3&& otherVector);
  [[nodiscard]] Vector3 copy() const;

  /**
   * @brief Creates a new Vector3 copied from the current Vector3.
   * @returns the new Vector3
   */
  [[nodiscard]] std::unique_ptr<Vector3> clone() const;

  /**
   * @brief Creates a string representation of the Vector3.
   * @returns a string with the Vector3 coordinates.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * @brief Gets the class name.
   * @returns the string "Vector3"
   */
  [[nodiscard]] const char* getClassName() const;

  /**
   * @brief Creates the Vector3 hash code.
   * @returns a number which tends to be unique between Vector3 instances
   */
  [[nodiscard]] size_t getHashCode() const;

  /** Operators **/

  /**
   * @brief Creates an array containing three elements : the coordinates of the
   * Vector3.
   * @returns a new array of numbers
   */
  [[nodiscard]] Float32Array asArray() const;

  /**
   * @brief Populates the given array or Float32Array from the given index with
   * the successive coordinates of the Vector3.
   * @param array defines the destination array
   * @param index defines the offset in the destination array
   * @returns the current Vector3
   */
  const Vector3& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Converts the current Vector3 into a quaternion (considering that the
   * Vector3 contains Euler angles representation of a rotation).
   * @returns a new Quaternion object, computed from the Vector3 coordinates
   */
  [[nodiscard]] Quaternion toQuaternion() const;

  /**
   * @brief Adds the given vector to the current Vector3.
   * @param otherVector defines the second operand
   * @returns the current updated Vector3
   */
  Vector3& addInPlace(const Vector3& otherVector);

  /**
   * @brief Adds the given coordinates to the current Vector3.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns the current updated Vector3
   */
  Vector3& addInPlaceFromFloats(float x, float y, float z);

  /**
   * @brief Gets a new Vector3, result of the addition the current Vector3 and
   * the given vector.
   * @param otherVector defines the second operand
   * @returns the resulting Vector3
   */
  [[nodiscard]] Vector3 add(const Vector3& otherVector) const;

  /**
   * @brief Adds the current Vector3 to the given one and stores the result in
   * the vector "result".
   * @param otherVector defines the second operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  Vector3& addToRef(const Vector3& otherVector, Vector3& result);

  /**
   * @brief Adds the current Vector3 to the given one and stores the result in
   * the vector "result".
   * @param otherVector defines the second operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  const Vector3& addToRef(const Vector3& otherVector, Vector3& result) const;

  /**
   * @brief Subtract the given vector from the current Vector3.
   * @param otherVector defines the second operand
   * @returns the current updated Vector3
   */
  Vector3& subtractInPlace(const Vector3& otherVector);

  /**
   * @brief Returns a new Vector3, result of the subtraction of the given vector
   * from the current Vector3.
   * @param otherVector defines the second operand
   * @returns the resulting Vector3
   */
  [[nodiscard]] Vector3 subtract(const Vector3& otherVector) const;

  /**
   * @brief Subtracts the given vector from the current Vector3 and stores the
   * result in the vector "result".
   * @param otherVector defines the second operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  Vector3& subtractToRef(const Vector3& otherVector, Vector3& result);

  /**
   * @brief Subtracts the given vector from the current Vector3 and stores the
   * result in the vector "result".
   * @param otherVector defines the second operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  const Vector3& subtractToRef(const Vector3& otherVector, Vector3& result) const;

  /**
   * @brief Returns a new Vector3 set with the subtraction of the given floats
   * from the current Vector3 coordinates.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns the resulting Vector3
   */
  [[nodiscard]] Vector3 subtractFromFloats(float x, float y, float z) const;

  /**
   * @brief Subtracts the given floats from the current Vector3 coordinates and
   * set the given vector "result" with this result.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  Vector3& subtractFromFloatsToRef(float x, float y, float z, Vector3& result);

  /**
   * @brief Subtracts the given floats from the current Vector3 coordinates and
   * set the given vector "result" with this result.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  const Vector3& subtractFromFloatsToRef(float x, float y, float z, Vector3& result) const;

  /**
   * @brief Gets a new Vector3 set with the current Vector3 negated coordinates.
   * @returns a new Vector3
   */
  [[nodiscard]] Vector3 negate() const;

  /**
   * @brief Multiplies the Vector3 coordinates by the float "scale".
   * @param scale defines the multiplier factor
   * @returns the current updated Vector3
   */
  Vector3& scaleInPlace(float iscale);

  /**
   * @brief Returns a new Vector3 set with the current Vector3 coordinates
   * multiplied by the float "scale".
   * @param scale defines the multiplier factor
   * @returns a new Vector3
   */
  [[nodiscard]] Vector3 scale(float iscale) const;

  /**
   * @brief Multiplies the current Vector3 coordinates by the float "scale" and
   * stores the result in the given vector "result" coordinates.
   * @param scale defines the multiplier factor
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  const Vector3& scaleToRef(int iscale, Vector3& result) const;

  /**
   * @brief Multiplies the current Vector3 coordinates by the float "scale" and
   * stores the result in the passed vector "result" coordinates.
   * @param scale defines the multiplier factor
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  const Vector3& scaleToRef(float iscale, Vector3& result) const;

  /**
   * @brief Scale the current Vector3 values by a factor and add the result to a
   * given Vector3.
   * @param scale defines the scale factor
   * @param result defines the Vector3 object where to store the result
   * @returns the unmodified current Vector3
   */
  const Vector3& scaleAndAddToRef(int iscale, Vector3& result) const;

  /**
   * @brief Scale the current Vector3 values by a factor and add the result to a
   * given Vector3.
   * @param scale defines the scale factor
   * @param result defines the Vector3 object where to store the result
   * @returns the unmodified current Vector3
   */
  const Vector3& scaleAndAddToRef(float iscale, Vector3& result) const;

  /**
   * @brief Returns true if the current Vector3 and the given vector coordinates
   * are strictly equal.
   * @param otherVector defines the second operand
   * @returns true if both vectors are equals
   */
  [[nodiscard]] bool equals(const Vector3& otherVector) const;

  /**
   * @brief Returns true if the current Vector3 and the given vector coordinates
   * are distant less than epsilon.
   * @param otherVector defines the second operand
   * @param epsilon defines the minimal distance to define values as equals
   * @returns true if both vectors are distant less than epsilon
   */
  [[nodiscard]] bool equalsWithEpsilon(const Vector3& otherVector,
                                       float epsilon = Math::Epsilon) const;

  /**
   * @brief Returns true if the current Vector3 coordinates equals the given
   * floats.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns true if both vectors are equals
   */
  [[nodiscard]] bool equalsToFloats(float x, float y, float z) const;

  /**
   * @brief Multiplies the current Vector3 coordinates by the given ones.
   * @param otherVector defines the second operand
   * @returns the current updated Vector3
   */
  Vector3& multiplyInPlace(const Vector3& otherVector);

  /**
   * @brief Returns a new Vector3, result of the multiplication of the current
   * Vector3 by the given vector.
   * @param otherVector defines the second operand
   * @returns the new Vector3
   */
  [[nodiscard]] Vector3 multiply(const Vector3& otherVector) const;

  /**
   * @brief Multiplies the current Vector3 by the given one and stores the
   * result in the given vector "result".
   * @param otherVector defines the second operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  const Vector3& multiplyToRef(const Vector3& otherVector, Vector3& result) const;

  /**
   * @brief Returns a new Vector3 set with the result of the mulliplication of
   * the current Vector3 coordinates by the given floats.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns the new Vector3
   */
  [[nodiscard]] Vector3 multiplyByFloats(float x, float y, float z) const;

  /**
   * @brief Returns a new Vector3 set with the result of the division of the
   * current Vector3 coordinates by the given ones.
   * @param otherVector defines the second operand
   * @returns the new Vector3
   */
  [[nodiscard]] Vector3 divide(const Vector3& otherVector) const;

  /**
   * @brief Divides the current Vector3 coordinates by the given ones and stores
   * the result in the given vector "result".
   * @param otherVector defines the second operand
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector3
   */
  Vector3& divideToRef(const Vector3& otherVector, Vector3& result) const;

  /**
   * @brief Divides the current Vector3 coordinates by the given ones.
   * @param otherVector defines the second operand
   * @returns the current updated Vector3
   */
  Vector3& divideInPlace(const Vector3& otherVector);

  /**
   * @brief Updates the current Vector3 with the minimal coordinate values
   * between its and the given vector ones.
   * @param other defines the second operand
   * @returns the current updated Vector3
   */
  Vector3& minimizeInPlace(const Vector3& other);

  /**
   * @brief Updates the current Vector3 with the maximal coordinate values
   * between its and the given vector ones.
   * @param other defines the second operand
   * @returns the current updated Vector3
   */
  Vector3& maximizeInPlace(const Vector3& other);

  /**
   * @brief Updates the current Vector3 with the minimal coordinate values
   * between its and the given coordinates.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns the current updated Vector3
   */
  Vector3& minimizeInPlaceFromFloats(float x, float y, float z);

  /**
   * @brief Updates the current Vector3 with the maximal coordinate values
   * between its and the given coordinates.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns the current updated Vector3
   */
  Vector3& maximizeInPlaceFromFloats(float x, float y, float z);

  /**
   * @brief Due to float precision, scale of a mesh could be uniform but float
   * values are off by a small fraction Check if is non uniform within a certain
   * amount of decimal places to account for this.
   * @param epsilon the amount the values can differ
   * @returns if the the vector is non uniform to a certain number of decimal
   * places
   */
  bool isNonUniformWithinEpsilon(float epsilon);

  /**
   * @brief Gets a boolean indicating that the vector is non uniform meaning x,
   * y or z are not all the same.
   */
  [[nodiscard]] bool isNonUniform() const;

  /**
   * @brief Gets a new Vector3 from current Vector3 floored values.
   * @returns a new Vector3
   */
  [[nodiscard]] Vector3 floor() const;

  /**
   * @brief Gets a new Vector3 from current Vector3 floored values.
   * @returns a new Vector3
   */
  [[nodiscard]] Vector3 fract() const;

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
  Vector3 operator+(const Vector3& otherVector) const;
  Vector3& operator+=(const Vector3& otherVector);
  Vector3 operator-(const Vector3& otherVector) const;
  Vector3& operator-=(const Vector3& otherVector);
  Vector3 operator-() const;
  Vector3 operator*(float scaleVal) const;
  Vector3& operator*=(float scaleVal);
  Vector3 operator*(const Vector3& otherVector) const;
  Vector3& operator*=(const Vector3& otherVector);
  Vector3 operator/(float value) const;
  Vector3 operator/(const Vector3& otherVector) const;
  Vector3& operator/=(float scaleVal);
  Vector3& operator/=(const Vector3& otherVector);
  bool operator==(const Vector3& otherVector) const;
  bool operator!=(const Vector3& otherVector) const;
  const float& operator[](unsigned int index) const;

  /** Properties **/

  /**
   * @brief Gets the length of the Vector3.
   * @returns the length of the Vecto3
   */
  [[nodiscard]] float length() const;

  /**
   * @brief Gets the squared length of the Vector3.
   * @returns squared length of the Vector3
   */
  [[nodiscard]] float lengthSquared() const;

  /** Methods **/

  /**
   * @brief Normalize the current Vector3.
   * Please note that this is an in place operation.
   * @returns the current updated Vector3
   */
  Vector3& normalize();

  /**
   * @brief Reorders the x y z properties of the vector in place.
   * @param order new ordering of the properties (eg. for vector 1,2,3 with
   * "ZYX" will produce 3,2,1)
   * @returns the current updated vector
   */
  Vector3& reorderInPlace(std::string order);

  /**
   * @brief Rotates the vector around 0,0,0 by a quaternion.
   * @param quaternion the rotation quaternion
   * @param result vector to store the result
   * @returns the resulting vector
   */
  Vector3& rotateByQuaternionToRef(const Quaternion& quaternion, Vector3& result);

  /**
   * @brief Rotates a vector around a given point.
   * @param quaternion the rotation quaternion
   * @param point the point to rotate around
   * @param result vector to store the result
   * @returns the resulting vector
   */
  Vector3& rotateByQuaternionAroundPointToRef(const Quaternion& quaternion, const Vector3& point,
                                              Vector3& result);

  /**
   * @brief Normalize the current Vector3 with the given input length.
   * Please note that this is an in place operation.
   * @param len the length of the vector
   * @returns the current updated Vector3
   */
  Vector3& normalizeFromLength(float len);

  /**
   * @brief Normalize the current Vector3 to a new vector.
   * @returns the new Vector3
   */
  [[nodiscard]] Vector3 normalizeToNew() const;

  /**
   * @brief Normalize the current Vector3 to the reference.
   * @param reference define the Vector3 to update
   * @returns the updated Vector3
   */
  Vector3 normalizeToRef(Vector3& reference) const;

  /**
   * @brief Copies the given vector coordinates to the current Vector3 ones.
   * @param source defines the source Vector3
   * @returns the current updated Vector3
   */
  Vector3& copyFrom(const Vector3& source);

  /**
   * @brief Copies the given floats to the current Vector3 coordinates.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns the current updated Vector3
   */
  Vector3& copyFromFloats(float x, float y, float z);

  /**
   * @brief Copies the given floats to the current Vector3 coordinates.
   * @param x defines the x coordinate of the operand
   * @param y defines the y coordinate of the operand
   * @param z defines the z coordinate of the operand
   * @returns the current updated Vector3
   */
  Vector3& set(float x, float y, float z);

  /**
   * @brief Copies the given float to the current Vector3 coordinates.
   * @param v defines the x, y and z coordinates of the operand
   * @returns the current updated Vector3
   */
  Vector3& setAll(float v);

  /** Statics **/

  /**
   * @brief Get the clip factor between two vectors.
   * @param vector0 defines the first operand
   * @param vector1 defines the second operand
   * @param axis defines the axis to use
   * @param size defines the size along the axis
   * @returns the clip factor
   */
  static float GetClipFactor(const Vector3& vector0, const Vector3& vector1, const Vector3& axis,
                             float size);

  /**
   * @brief Get angle between two vectors.
   * @param vector0 angle between vector0 and vector1
   * @param vector1 angle between vector0 and vector1
   * @param normal direction of the normal
   * @return the angle between vector0 and vector1
   */
  static float GetAngleBetweenVectors(const Vector3& vector0, const Vector3& vector1,
                                      const Vector3& normal);

  /**
   * @brief Returns a new Vector3 set from the index "offset" of the given
   * array.
   * @param array defines the source array
   * @param offset defines the offset in the source array
   * @returns the new Vector3
   */
  static Vector3 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Sets the given vector "result" with the element values from the
   * index "offset" of the given array.
   * @param array defines the source array
   * @param offset defines the offset in the source array
   * @param result defines the Vector3 where to store the result
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset, Vector3& result);

  /**
   * @brief Sets the given vector "result" with the element values from the
   * index "offset" of the given array.
   * @param array defines the source array
   * @param offset defines the offset in the source array
   * @param result defines the Vector3 where to store the result
   */
  static void FromArrayToRef(const std::array<float, 16>& array, unsigned int offset,
                             Vector3& result);

  /**
   * @brief Sets the given vector "result" with the element values from the
   * index "offset" of the given Float32Array. This function is deprecated.  Use
   * FromArrayToRef instead.
   * @param array defines the source array
   * @param offset defines the offset in the source array
   * @param result defines the Vector3 where to store the result
   */
  static void FromFloatArrayToRef(const Float32Array& array, unsigned int offset, Vector3& result);

  /**
   * @brief Sets the given vector "result" with the element values from the
   * index "offset" of the given Float32Array. This function is deprecated.  Use
   * FromArrayToRef instead.
   * @param array defines the source array
   * @param offset defines the offset in the source array
   * @param result defines the Vector3 where to store the result
   */
  static void FromFloatArrayToRef(const std::array<float, 16>& array, unsigned int offset,
                                  Vector3& result);

  /**
   * Sets the given vector "result" with the given floats.
   * @param x defines the x coordinate of the source
   * @param y defines the y coordinate of the source
   * @param z defines the z coordinate of the source
   * @param result defines the Vector3 where to store the result
   */
  static void FromFloatsToRef(float ix, float iy, float iz, Vector3& result);

  /**
   * @brief Returns a new Vector3 set to (0.0, 0.0, 0.0).
   * @returns a new empty Vector3
   */
  static Vector3 Zero();

  /**
   * @brief Returns a new Vector3 set to (1.0, 1.0, 1.0).
   * @returns a new unit Vector3
   */
  static Vector3 One();

  /**
   * @brief Returns a new Vector3 set to (0.0, 1.0, 0.0).
   * @returns a new up Vector3
   */
  static Vector3 Up();

  /**
   * @brief Gets a up Vector3 that must not be updated.
   */
  static Vector3 UpReadOnly();

  /**
   * @brief Returns a new Vector3 set to (0.0, -1.0, 0.0).
   * @returns a new down Vector3
   */
  static Vector3 Down();

  /**
   * @brief Returns a new Vector3 set to (0.0, 0.0, 1.0).
   * @returns a new forward Vector3
   */
  static Vector3 Forward();

  /**
   * @brief Returns a new Vector3 set to (0.0, 0.0, -1.0).
   * @returns a new forward Vector3
   */
  static Vector3 Backward();

  /**
   * @brief Returns a new Vector3 set to (1.0, 0.0, 0.0).
   * @returns a new right Vector3
   */
  static Vector3 Right();

  /**
   * @brief Returns a new Vector3 set to (-1.0, 0.0, 0.0).
   * @returns a new left Vector3
   */
  static Vector3 Left();

  /**
   * @brief Returns a new Vector3 set with the result of the transformation by
   * the given matrix of the given vector. This method computes tranformed
   * coordinates only, not transformed direction vectors (ie. it takes
   * translation in account).
   * @param vector defines the Vector3 to transform
   * @param transformation defines the transformation matrix
   * @returns the transformed Vector3
   */
  static Vector3 TransformCoordinates(const Vector3& vector, const Matrix& transformation);

  /**
   * @brief Sets the given vector "result" coordinates with the result of the
   * transformation by the given matrix of the given vector. This method
   * computes tranformed coordinates only, not transformed direction vectors
   * (ie. it takes translation in account).
   * @param vector defines the Vector3 to transform
   * @param transformation defines the transformation matrix
   * @param result defines the Vector3 where to store the result
   */
  static void TransformCoordinatesToRef(const Vector3& vector, const Matrix& transformation,
                                        Vector3& result);

  /**
   * @brief Sets the given vector "result" coordinates with the result of the
   * transformation by the given matrix of the given floats (x, y, z). This
   * method computes tranformed coordinates only, not transformed direction
   * vectors.
   * @param x define the x coordinate of the source vector
   * @param y define the y coordinate of the source vector
   * @param z define the z coordinate of the source vector
   * @param transformation defines the transformation matrix
   * @param result defines the Vector3 where to store the result
   */
  static void TransformCoordinatesFromFloatsToRef(float x, float y, float z,
                                                  const Matrix& transformation, Vector3& result);

  /**
   * @brief Returns a new Vector3 set with the result of the normal
   * transformation by the given matrix of the given vector. This methods
   * computes transformed normalized direction vectors only (ie. it does not
   * apply translation).
   * @param vector defines the Vector3 to transform
   * @param transformation defines the transformation matrix
   * @returns the new Vector3
   */
  static Vector3 TransformNormal(const Vector3& vector, const Matrix& transformation);

  /**
   * @brief Sets the given vector "result" with the result of the normal
   * transformation by the given matrix of the given vector. This methods
   * computes transformed normalized direction vectors only (ie. it does not
   * apply translation).
   * @param vector defines the Vector3 to transform
   * @param transformation defines the transformation matrix
   * @param result defines the Vector3 where to store the result
   */
  static void TransformNormalToRef(const Vector3& vector, const Matrix& transformation,
                                   Vector3& result);

  /**
   * @brief Sets the given vector "result" with the result of the normal
   * transformation by the given matrix of the given floats (x, y, z) This
   * methods computes transformed normalized direction vectors only (ie. it does
   * not apply translation).
   * @param x define the x coordinate of the source vector
   * @param y define the y coordinate of the source vector
   * @param z define the z coordinate of the source vector
   * @param transformation defines the transformation matrix
   * @param result defines the Vector3 where to store the result
   */
  static void TransformNormalFromFloatsToRef(float x, float y, float z,
                                             const Matrix& transformation, Vector3& result);

  /**
   * @brief Returns a new Vector3 located for "amount" on the CatmullRom
   * interpolation spline defined by the vectors "value1", "value2", "value3",
   * "value4".
   * @param value1 defines the first control point
   * @param value2 defines the second control point
   * @param value3 defines the third control point
   * @param value4 defines the fourth control point
   * @param amount defines the amount on the spline to use
   * @returns the new Vector3
   */
  static Vector3 CatmullRom(const Vector3& value1, const Vector3& value2, const Vector3& value3,
                            const Vector3& value4, float amount);

  /**
   * @brief Returns a new Vector3 set with the coordinates of "value", if the
   * vector "value" is in the cube defined by the vectors "min" and "max". If a
   * coordinate value of "value" is lower than one of the "min" coordinate, then
   * this "value" coordinate is set with the "min" one. If a coordinate value of
   * "value" is greater than one of the "max" coordinate, then this "value"
   * coordinate is set with the "max" one.
   * @param value defines the current value
   * @param min defines the lower range value
   * @param max defines the upper range value
   * @returns the new Vector3
   */
  static Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max);

  /**
   * @brief Sets the given vector "result" with the coordinates of "value", if
   * the vector "value" is in the cube defined by the vectors "min" and "max" If
   * a coordinate value of "value" is lower than one of the "min" coordinate,
   * then this "value" coordinate is set with the "min" one If a coordinate
   * value of "value" is greater than one of the "max" coordinate, then this
   * "value" coordinate is set with the "max" one
   * @param value defines the current value
   * @param min defines the lower range value
   * @param max defines the upper range value
   * @param result defines the Vector3 where to store the result
   */
  static void ClampToRef(const Vector3& value, const Vector3& min, const Vector3& max,
                         Vector3& result);

  /**
   * @brief Checks if a given vector is inside a specific range.
   * @param v defines the vector to test
   * @param min defines the minimum range
   * @param max defines the maximum range
   */
  static void CheckExtends(Vector3& v, Vector3& min, Vector3& max);

  /**
   * @brief Returns a new Vector3 located for "amount" (float) on the Hermite
   * interpolation spline defined by the vectors "value1", "tangent1", "value2",
   * "tangent2".
   * @param value1 defines the first control point
   * @param tangent1 defines the first tangent vector
   * @param value2 defines the second control point
   * @param tangent2 defines the second tangent vector
   * @param amount defines the amount on the interpolation spline (between 0 and
   * 1)
   * @returns the new Vector3
   */
  static Vector3 Hermite(const Vector3& value1, const Vector3& tangent1, const Vector3& value2,
                         const Vector3& tangent2, float amount);

  /**
   * @brief Returns a new Vector3 located for "amount" (float) on the linear
   * interpolation between the vectors "start" and "end".
   * @param start defines the start value
   * @param end defines the end value
   * @param amount max defines amount between both (between 0 and 1)
   * @returns the new Vector3
   */
  static Vector3 Lerp(const Vector3& start, const Vector3& end, float amount);

  /**
   * @brief Sets the given vector "result" with the result of the linear
   * interpolation from the vector "start" for "amount" to the vector "end".
   * @param start defines the start value
   * @param end defines the end value
   * @param amount max defines amount between both (between 0 and 1)
   * @param result defines the Vector3 where to store the result
   */
  static void LerpToRef(const Vector3& start, const Vector3& end, float amount, Vector3& result);

  /**
   * @brief Returns the dot product (float) between the vectors "left" and
   * "right".
   * @param left defines the left operand
   * @param right defines the right operand
   * @returns the dot product
   */
  static float Dot(const Vector3& left, const Vector3& right);

  /**
   * @brief Returns a new Vector3 as the cross product of the vectors "left" and
   * "right". The cross product is then orthogonal to both "left" and "right".
   * @param left defines the left operand
   * @param right defines the right operand
   * @returns the cross product
   */
  static Vector3 Cross(const Vector3& left, const Vector3& right);

  /**
   * @brief Sets the given vector "result" with the cross product of "left" and
   * "right". The cross product is then orthogonal to both "left" and "right"
   * @param left defines the left operand
   * @param right defines the right operand
   * @param result defines the Vector3 where to store the result
   */
  static void CrossToRef(const Vector3& left, const Vector3& right, Vector3& result);

  /**
   * @brief Returns a new Vector3 as the normalization of the given vector.
   * @param vector defines the Vector3 to normalize
   * @returns the new Vector3
   */
  static Vector3 Normalize(const Vector3& vector);

  /**
   * @brief Sets the given vector "result" with the normalization of the given
   * first vector.
   * @param vector defines the Vector3 to normalize
   * @param result defines the Vector3 where to store the result
   */
  static void NormalizeToRef(const Vector3& vector, Vector3& result);

  /**
   * @brief Project a Vector3 onto screen space.
   * @param vector defines the Vector3 to project
   * @param world defines the world matrix to use
   * @param transform defines the transform (view x projection) matrix to use
   * @param viewport defines the screen viewport to use
   * @returns the new Vector3
   */
  static Vector3 Project(const Vector3& vector, Matrix& world, Matrix& transform,
                         const Viewport& viewport);

  /**
   * @brief Hidden
   */
  static void _UnprojectFromInvertedMatrixToRef(const Vector3& source, const Matrix& matrix,
                                                Vector3& result);

  /**
   * @brief Unproject from screen space to object space.
   * @param source defines the screen space Vector3 to use
   * @param viewportWidth defines the current width of the viewport
   * @param viewportHeight defines the current height of the viewport
   * @param world defines the world matrix to use (can be set to Identity to go
   * to world space)
   * @param transform defines the transform (view x projection) matrix to use
   * @returns the new Vector3
   */
  static Vector3 UnprojectFromTransform(Vector3& source, float viewportWidth, float viewportHeight,
                                        Matrix& world, Matrix& transform);

  /**
   * @brief Unproject from screen space to object space.
   * @param source defines the screen space Vector3 to use
   * @param viewportWidth defines the current width of the viewport
   * @param viewportHeight defines the current height of the viewport
   * @param world defines the world matrix to use (can be set to Identity to go
   * to world space)
   * @param view defines the view matrix to use
   * @param projection defines the projection matrix to use
   * @returns the new Vector3
   */
  static Vector3 Unproject(const Vector3& source, float viewportWidth, float viewportHeight,
                           Matrix& world, Matrix& view, Matrix& projection);

  /**
   * @brief Unproject from screen space to object space.
   * @param source defines the screen space Vector3 to use
   * @param viewportWidth defines the current width of the viewport
   * @param viewportHeight defines the current height of the viewport
   * @param world defines the world matrix to use (can be set to Identity to go
   * to world space)
   * @param view defines the view matrix to use
   * @param projection defines the projection matrix to use
   * @param result defines the Vector3 where to store the result
   */
  static void UnprojectToRef(const Vector3& source, float viewportWidth, float viewportHeight,
                             Matrix& world, Matrix& view, Matrix& projection, Vector3& result);

  /**
   * @brief Unproject from screen space to object space.
   * @param sourceX defines the screen space x coordinate to use
   * @param sourceY defines the screen space y coordinate to use
   * @param sourceZ defines the screen space z coordinate to use
   * @param viewportWidth defines the current width of the viewport
   * @param viewportHeight defines the current height of the viewport
   * @param world defines the world matrix to use (can be set to Identity to go
   * to world space)
   * @param view defines the view matrix to use
   * @param projection defines the projection matrix to use
   * @param result defines the Vector3 where to store the result
   */
  static void UnprojectFloatsToRef(float sourceX, float sourceY, float sourceZ, float viewportWidth,
                                   float viewportHeight, Matrix& world, Matrix& view,
                                   Matrix& projection, Vector3& result);

  /**
   * @brief Gets the minimal coordinate values between two Vector3.
   * @param left defines the first operand
   * @param right defines the second operand
   * @returns the new Vector3
   */
  static Vector3 Minimize(const Vector3& left, const Vector3& right);

  /**
   * @brief Gets the maximal coordinate values between two Vector3.
   * @param left defines the first operand
   * @param right defines the second operand
   * @returns the new Vector3
   */
  static Vector3 Maximize(const Vector3& left, const Vector3& right);

  /**
   * @brief Returns the distance between the vectors "value1" and "value2".
   * @param value1 defines the first operand
   * @param value2 defines the second operand
   * @returns the distance
   */
  static float Distance(const Vector3& value1, const Vector3& value2);

  /**
   * @brief Returns the squared distance between the vectors "value1" and
   * "value2".
   * @param value1 defines the first operand
   * @param value2 defines the second operand
   * @returns the squared distance
   */
  static float DistanceSquared(const Vector3& value1, const Vector3& value2);

  /**
   * @brief Returns a new Vector3 located at the center between "value1" and
   * "value2".
   * @param value1 defines the first operand
   * @param value2 defines the second operand
   * @returns the new Vector3
   */
  static Vector3 Center(const Vector3& value1, const Vector3& value2);

  /**
   * @brief Given three orthogonal normalized left-handed oriented Vector3 axis
   * in space (target system), RotationFromAxis() returns the rotation Euler
   * angles (ex : rotation.x, rotation.y, rotation.z) to apply to something in
   * order to rotate it from its local system to the given target system. Note:
   * axis1, axis2 and axis3 are normalized during this operation.
   * @param axis1 defines the first axis
   * @param axis2 defines the second axis
   * @param axis3 defines the third axis
   * @returns a new Vector3
   */
  static Vector3 RotationFromAxis(Vector3& axis1, Vector3& axis2, Vector3& axis3);

  /**
   * @brief The same than RotationFromAxis but updates the given ref Vector3
   * parameter instead of returning a new Vector3.
   * @param axis1 defines the first axis
   * @param axis2 defines the second axis
   * @param axis3 defines the third axis
   * @param ref defines the Vector3 where to store the result
   */
  static void RotationFromAxisToRef(Vector3& axis1, Vector3& axis2, Vector3& axis3, Vector3& ref);

public:
  /**
   * Defines the first coordinates (on X axis)
   */
  float x;

  /**
   * Defines the second coordinates (on Y axis)
   */
  float y;

  /**
   * Defines the third coordinates (on Z axis)
   */
  float z;

}; // end of class Vector3

std::ostream& operator<<(std::ostream& os, const Vector3& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR3_H
