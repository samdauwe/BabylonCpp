#ifndef BABYLON_MATHS_VECTOR2_H
#define BABYLON_MATHS_VECTOR2_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_constants.h>

namespace BABYLON {

class Matrix;
class Vector3;

/**
 * @brief Class representing a vector containing 2 coordinates.
 */
class BABYLON_SHARED_EXPORT Vector2 {

public:
  /**
   * @brief Creates a new Vector2 from the given x and y coordinates.
   * @param x defines the first coordinate
   * @param y defines the second coordinate
   */
  Vector2(float x = 0.f, float y = 0.f);
  Vector2(int x, int y);
  Vector2(const Vector2& otherVector);
  Vector2(Vector2&& otherVector);
  Vector2& operator=(const Vector2& otherVector);
  Vector2& operator=(Vector2&& otherVector);
  ~Vector2(); // = default
  [[nodiscard]] Vector2 copy() const;

  /**
   * @brief Gets a new Vector2 copied from the Vector2.
   * @returns a new Vector2
   */
  [[nodiscard]] std::unique_ptr<Vector2> clone() const;

  /**
   * @brief Gets a string with the Vector2 coordinates.
   * @returns a string with the Vector2 coordinates
   */
  [[nodiscard]] std::string toString() const;

  /**
   * @brief Gets class name.
   * @returns the string "Vector2"
   */
  [[nodiscard]] const char* getClassName() const;

  /**
   * @brief Gets current vector hash code.
   * @returns the Vector2 hash code as a number
   */
  [[nodiscard]] size_t getHashCode() const;

  /** Operators **/

  /**
   * @brief Sets the Vector2 coordinates in the given array or Float32Array from the given index.
   * @param array defines the source array
   * @param index defines the offset in source array
   * @returns the current Vector2
   */
  const Vector2& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Update the current vector from an array.
   * @param array defines the destination array
   * @param index defines the offset in the destination array
   * @returns the current Vector3
   */
  Vector2& fromArray(const Float32Array& array, unsigned int index = 0);

  /**
   * @brief Copy the current vector to an array.
   * @returns a new array with 2 elements: the Vector2 coordinates.
   */
  [[nodiscard]] Float32Array asArray() const;

  /**
   * @brief Sets the Vector2 coordinates with the given Vector2 coordinates.
   * @param source defines the source Vector2
   * @returns the current updated Vector2
   */
  Vector2& copyFrom(const Vector2& source);

  /**
   * @brief Sets the Vector2 coordinates with the given floats.
   * @param x defines the first coordinate
   * @param y defines the second coordinate
   * @returns the current updated Vector2
   */
  Vector2& copyFromFloats(float x, float y);

  /**
   * @brief Sets the Vector2 coordinates with the given floats.
   * @param x defines the first coordinate
   * @param y defines the second coordinate
   * @returns the current updated Vector2
   */
  Vector2& set(float x, float y);

  /**
   * @brief Add another vector with the current one
   * @param otherVector defines the other vector
   * @returns a new Vector2 set with the addition of the current Vector2 and the given one
   * coordinates.
   */
  [[nodiscard]] Vector2 add(const Vector2& otherVector) const;

  /**
   * @brief Sets the "result" coordinates with the addition of the current Vector2 and the given one
   * coordinates.
   * @param otherVector defines the other vector
   * @param result defines the target vector
   * @returns the unmodified current Vector2
   */
  const Vector2& addToRef(const Vector2& otherVector, Vector2& result) const;

  /**
   * @brief Set the Vector2 coordinates by adding the given Vector2 coordinates.
   * @param otherVector defines the other vector
   * @returns the current updated Vector2
   */
  Vector2& addInPlace(const Vector2& otherVector);

  /**
   * @brief Gets a new Vector2 by adding the current Vector2 coordinates to the given Vector3 x, y
   * coordinates.
   * @param otherVector defines the other vector
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 addVector3(const Vector3& otherVector) const;

  /**
   * @brief Gets a new Vector2 set with the subtracted coordinates of the given one from the current
   * Vector2.
   * @param otherVector defines the other vector
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 subtract(const Vector2& otherVector) const;

  /**
   * @brief Sets the "result" coordinates with the subtraction of the passed one. from the current
   * Vector2 coordinates.
   * @returns The Vector2.
   */
  const Vector2& subtractToRef(const Vector2& otherVector, Vector2& result) const;

  /**
   * @brief Sets the "result" coordinates with the subtraction of the given one from the current
   * Vector2 coordinates.
   * @param otherVector defines the other vector
   * @param result defines the target vector
   * @returns the unmodified current Vector2
   */
  Vector2& subtractInPlace(const Vector2& otherVector);

  /**
   * @brief Multiplies in place the current Vector2 coordinates by the given ones.
   * @param otherVector defines the other vector
   * @returns the current updated Vector2
   */
  Vector2& multiplyInPlace(const Vector2& otherVector);

  /**
   * @brief Returns a new Vector2 set with the multiplication of the current Vector2 and the given
   * one coordinates.
   * @param otherVector defines the other vector
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 multiply(const Vector2& otherVector) const;

  /**
   * @brief Sets "result" coordinates with the multiplication of the current Vector2 and the given
   * one coordinates.
   * @param otherVector defines the other vector
   * @param result defines the target vector
   * @returns the unmodified current Vector2
   */
  const Vector2& multiplyToRef(const Vector2& otherVector, Vector2& result) const;

  /**
   * @brief Gets a new Vector2 set with the Vector2 coordinates multiplied by the given floats.
   * @param x defines the first coordinate
   * @param y defines the second coordinate
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 multiplyByFloats(float x, float y) const;

  /**
   * @brief Returns a new Vector2 set with the Vector2 coordinates divided by the given one
   * coordinates.
   * @param otherVector defines the other vector
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 divide(const Vector2& otherVector) const;

  /**
   * @brief Sets the "result" coordinates with the Vector2 divided by the given one coordinates.
   * @param otherVector defines the other vector
   * @param result defines the target vector
   * @returns the unmodified current Vector2
   */
  const Vector2& divideToRef(const Vector2& otherVector, Vector2& result) const;

  /**
   * @brief Divides the current Vector2 coordinates by the given ones.
   * @param otherVector defines the other vector
   * @returns the current updated Vector2
   */
  Vector2& divideInPlace(const Vector2& otherVector);

  /**
   * @brief Gets a new Vector2 with current Vector2 negated coordinates.
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 negate() const;

  /**
   * @brief Negate this vector in place.
   * @returns this
   */
  Vector2& negateInPlace();

  /**
   * @brief Negate the current Vector2 and stores the result in the given vector "result"
   * coordinates.
   * @param result defines the Vector3 object where to store the result
   * @returns the current Vector2
   */
  Vector2 negateToRef(Vector2& result);

  /**
   * @brief Multiply the Vector2 coordinates by scale.
   * @param scale defines the scaling factor
   * @returns the current updated Vector2
   */
  Vector2& scaleInPlace(float scaleVal);

  /**
   * @brief Returns a new Vector2 scaled by "scale" from the current Vector2.
   * @param scale defines the scaling factor
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 scale(float iScale) const;

  /**
   * @brief Scale the current Vector2 values by a factor to a given Vector2.
   * @param scale defines the scale factor
   * @param result defines the Vector2 object where to store the result
   * @returns the unmodified current Vector2
   */
  const Vector2& scaleToRef(float iScale, Vector2& result) const;

  /**
   * @brief Scale the current Vector2 values by a factor and add the result to a given Vector2.
   * @param scale defines the scale factor
   * @param result defines the Vector2 object where to store the result
   * @returns the unmodified current Vector2
   */
  const Vector2& scaleAndAddToRef(float iScale, Vector2& result) const;

  /**
   * @brief Gets a boolean if two vectors are equals.
   * @param otherVector defines the other vector
   * @returns true if the given vector coordinates strictly equal the current Vector2 ones
   */
  [[nodiscard]] bool equals(const Vector2& otherVector) const;

  /**
   * @brief Gets a boolean if two vectors are equals (using an epsilon value).
   * @param otherVector defines the other vector
   * @param epsilon defines the minimal distance to consider equality
   * @returns true if the given vector coordinates are close to the current ones by a distance of
   * epsilon.
   */
  [[nodiscard]] bool equalsWithEpsilon(const Vector2& otherVector,
                                       float epsilon = Math::Epsilon) const;

  /**
   * @brief Gets a new Vector2 from current Vector2 floored values.
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 floor() const;

  /**
   * @brief  Gets a new Vector2 from current Vector2 floored values.
   * @returns a new Vector2
   */
  [[nodiscard]] Vector2 fract() const;

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const Vector2& vector);
  Vector2 operator+(const Vector2& otherVector) const;
  Vector2 operator+(const Vector3& otherVector) const;
  Vector2& operator++();
  Vector2 operator++(int);
  Vector2 operator+=(const Vector2& otherVector);
  Vector2 operator-(const Vector2& otherVector) const;
  Vector2& operator-=(const Vector2& otherVector);
  Vector2& operator--();
  Vector2 operator--(int);
  Vector2 operator*(const Vector2& otherVector) const;
  Vector2& operator*=(const Vector2& otherVector);
  Vector2 operator*(float scaleVal) const;
  Vector2& operator*=(float scaleVal);
  Vector2 operator/(const Vector2& otherVector) const;
  Vector2& operator/=(const Vector2& otherVector);
  Vector2& operator/=(float scaleVal);
  Vector2 operator-() const;
  bool operator==(const Vector2& otherVector) const;
  bool operator!=(const Vector2& otherVector) const;
  bool operator<(const Vector2& otherVector) const;
  bool operator>(const Vector2& otherVector) const;
  bool operator<=(const Vector2& otherVector) const;
  bool operator>=(const Vector2& otherVector) const;

  /** Properties **/

  /**
   * @brief Gets the length of the vector.
   * @returns the vector length (float)
   */
  [[nodiscard]] float length() const;

  /**
   * @brief Gets the vector squared length.
   * @returns the vector squared length (float)
   */
  [[nodiscard]] float lengthSquared() const;

  /** Methods **/

  /**
   * @brief Normalize the vector.
   * @returns the current updated Vector2
   */
  Vector2& normalize();

  /** Statics **/

  /**
   * @brief Gets a new Vector2(0, 0).
   * @returns a new Vector2
   */
  static Vector2 Zero();

  /**
   * @brief Gets a new Vector2(1, 1).
   * @returns a new Vector2
   */
  static Vector2 One();

  /**
   * @brief Gets a new Vector2 set from the given index element of the given array.
   * @param array defines the data source
   * @param offset defines the offset in the data source
   * @returns a new Vector2
   */
  static Vector2 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Sets "result" from the given index element of the given array.
   * @param array defines the data source
   * @param offset defines the offset in the data source
   * @param result defines the target vector
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset, Vector2& result);

  /**
   * @brief Gets a new Vector2 located for "amount" (float) on the CatmullRom spline defined by the
   * given four Vector2.
   * @param value1 defines 1st point of control
   * @param value2 defines 2nd point of control
   * @param value3 defines 3rd point of control
   * @param value4 defines 4th point of control
   * @param amount defines the interpolation factor
   * @returns a new Vector2
   */
  static Vector2 CatmullRom(const Vector2& value1, const Vector2& value2, const Vector2& value3,
                            const Vector2& value4, float amount);

  /**
   * @brief Returns a new Vector2 set with same the coordinates than "value" ones if the vector
   * "value" is in the square defined by "min" and "max". If a coordinate of "value" is lower than
   * "min" coordinates, the returned Vector2 is given this "min" coordinate. If a coordinate of
   * "value" is greater than "max" coordinates, the returned Vector2 is given this "max" coordinate
   * @param value defines the value to clamp
   * @param min defines the lower limit
   * @param max defines the upper limit
   * @returns a new Vector2
   */
  static Vector2 Clamp(const Vector2& value, const Vector2& min, const Vector2& max);

  /**
   * @brief Returns a new Vector2 located for "amount" (float) on the Hermite spline defined by the
   * vectors "value1", "value3", "tangent1", "tangent2".
   * @param value1 defines the 1st control point
   * @param tangent1 defines the outgoing tangent
   * @param value2 defines the 2nd control point
   * @param tangent2 defines the incoming tangent
   * @param amount defines the interpolation factor
   * @returns a new Vector2
   */
  static Vector2 Hermite(const Vector2& value1, const Vector2& tangent1, const Vector2& value2,
                         const Vector2& tangent2, float amount);

  /**
   * @brief Returns a new Vector2 located for "amount" (float) on the linear interpolation between
   * the vector "start" adn the vector "end".
   * @param start defines the start vector
   * @param end defines the end vector
   * @param amount defines the interpolation factor
   * @returns a new Vector2
   */
  static Vector2 Lerp(const Vector2& start, const Vector2& end, float amount);

  /**
   * @brief Gets the dot product of the vector "left" and the vector "right".
   * @param left defines first vector
   * @param right defines second vector
   * @returns the dot product (float)
   */
  static float Dot(const Vector2& left, const Vector2& right);

  /**
   * @brief Returns a new Vector2 equal to the normalized given vector.
   * @param vector defines the vector to normalize
   * @returns a new Vector2
   */
  static Vector2 Normalize(Vector2& vector);

  /**
   * @brief Gets a new Vector2 set with the minimal coordinate values from the "left" and "right"
   * vectors.
   * @param left defines 1st vector
   * @param right defines 2nd vector
   * @returns a new Vector2
   */
  static Vector2 Minimize(const Vector2& left, const Vector2& right);

  /**
   * @brief Gets a new Vecto2 set with the maximal coordinate values from the "left" and "right"
   * vectors.
   * @param left defines 1st vector
   * @param right defines 2nd vector
   * @returns a new Vector2
   */
  static Vector2 Maximize(const Vector2& left, const Vector2& right);

  /**
   * @brief Gets a new Vector2 set with the transformed coordinates of the given vector by the given
   * transformation matrix.
   * @param vector defines the vector to transform
   * @param transformation defines the matrix to apply
   * @returns a new Vector2
   */
  static Vector2 Transform(const Vector2& vector, const Matrix& transformation);

  /**
   * @brief Transforms the given vector coordinates by the given transformation matrix and stores
   * the result in the vector "result" coordinates.
   * @param vector defines the vector to transform
   * @param transformation defines the matrix to apply
   * @param result defines the target vector
   */
  static void TransformToRef(const Vector2& vector, const Matrix& transformation, Vector2& result);

  /**
   * @brief Determines if a given vector is included in a triangle.
   * @param p defines the vector to test
   * @param p0 defines 1st triangle point
   * @param p1 defines 2nd triangle point
   * @param p2 defines 3rd triangle point
   * @returns true if the point "p" is in the triangle defined by the vertors "p0", "p1", "p2"
   */
  static bool PointInTriangle(const Vector2& p, const Vector2& p0, const Vector2& p1,
                              const Vector2& p2);

  /**
   * @brief Gets the distance between the vectors "value1" and "value2"
   * @param value1 defines first vector
   * @param value2 defines second vector
   * @returns the distance between vectors
   */
  static float Distance(const Vector2& value1, const Vector2& value2);

  /**
   * @brief Returns the squared distance between the vectors "value1" and "value2".
   * @param value1 defines first vector
   * @param value2 defines second vector
   * @returns the squared distance between vectors
   */
  static float DistanceSquared(const Vector2& value1, const Vector2& value2);

  /**
   * @brief Gets a new Vector2 located at the center of the vectors "value1" and "value2".
   * @param value1 defines first vector
   * @param value2 defines second vector
   * @returns a new Vector2
   */
  static Vector2 Center(const Vector2& value1, const Vector2& value2);

  /**
   * @brief Gets the center of the vectors "value1" and "value2" and stores the result in the vector
   * "ref".
   * @param value1 defines first vector
   * @param value2 defines second vector
   * @param ref defines third vector
   * @returns ref
   */
  static Vector2& CenterToRef(const Vector2& value1, const Vector2& value2, Vector2& ref);

  /**
   * @brief Gets the shortest distance (float) between the point "p" and the segment defined by the
   * two points "segA" and "segB".
   * @param p defines the middle point
   * @param segA defines one point of the segment
   * @param segB defines the other point of the segment
   * @returns the shortest distance
   */
  static float DistanceOfPointFromSegment(const Vector2& p, const Vector2& segA,
                                          const Vector2& segB);

public:
  /**
   * Defines the first coordinate
   */
  float x;

  /**
   * Defines the second coordinate
   */
  float y;

}; // end of class Vector2

std::ostream& operator<<(std::ostream& os, const Vector2& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_VECTOR2_H
