#ifndef BABYLON_MATH_VECTOR2_H
#define BABYLON_MATH_VECTOR2_H

#include <babylon/babylon_global.h>
#include <babylon/math/math_tools.h>

namespace BABYLON {

/**
 * @brief Represents a Vector2.
 */
class BABYLON_SHARED_EXPORT Vector2 {

public:
  /**
   * @brief Creates a new Vector2 from the passed x and y coordinates.
   */
  Vector2(float x = 0.f, float y = 0.f);
  Vector2(const Vector2& otherVector);
  Vector2(Vector2&& otherVector);
  Vector2& operator=(const Vector2& otherVector);
  Vector2& operator=(Vector2&& otherVector);
  ~Vector2();
  Vector2 copy() const;

  /**
   * @brief Returns a new Vector2 copied from the Vector2.
   */
  std::unique_ptr<Vector2> clone() const;

  /**
   * @brief Returns a string with the Vector2 coordinates.
   */
  std::string toString() const;

  /**
   * @brief Returns the string "Vector2".
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Vector2 hash code as a number.
   */
  size_t getHashCode() const;

  /** Operators **/

  /**
   * @brief Sets the Vector2 coordinates in the passed array or Float32Array
   * from the passed index.
   * @returns The Vector2.
   */
  const Vector2& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Returns a new array with 2 elements : the Vector2 coordinates.
   */
  Float32Array asArray() const;

  /**
   * @brief Sets the Vector2 coordinates with the passed Vector2 coordinates.
   * @returns The updated Vector2.
   */
  Vector2& copyFrom(const Vector2& source);

  /**
   * @brief Sets the Vector2 coordinates with the passed floats.
   * @returns The updated Vector2.
   */
  Vector2& copyFromFloats(float x, float y);

  /**
   * @brief Sets the Vector2 coordinates with the passed floats.
   * @returns the updated Vector2.
   */
  Vector2& set(float x, float y);

  /**
   * @brief Returns a new Vector2 set with the addition of the current Vector2
   * and the passed one coordinates.
   */
  Vector2 add(const Vector2& otherVector) const;

  /**
   * @brief Sets the "result" coordinates with the addition of the current
   * Vector2 and the passed one coordinates.
   * @returns The Vector2.
   */
  const Vector2& addToRef(const Vector2& otherVector, Vector2& result) const;

  /**
   * @brief Set the Vector2 coordinates by adding the passed Vector2
   * coordinates.
   * @returns The updated Vector2.
   */
  Vector2& addInPlace(const Vector2& otherVector);

  /**
   * @brief Returns a new Vector2 by adding the current Vector2 coordinates to
   * the passed Vector3 x, y coordinates.
   */
  Vector2 addVector3(const Vector3& otherVector) const;

  /**
   * @brief Returns a new Vector2 set with the subtracted coordinates of the
   * passed one from the current Vector2.
   */
  Vector2 subtract(const Vector2& otherVector) const;

  /**
   * @brief Sets the "result" coordinates with the subtraction of the passed one
   * from the current Vector2 coordinates.
   * @returns The Vector2.
   */
  const Vector2& subtractToRef(const Vector2& otherVector,
                               Vector2& result) const;

  /**
   * @brief Sets the current Vector2 coordinates by subtracting from it the
   * passed one coordinates.
   * @returns The updated Vector2.
   */
  Vector2& subtractInPlace(const Vector2& otherVector);

  /**
   * @brief Multiplies in place the current Vector2 coordinates by the passed
   * ones.
   * @returns The updated Vector2.
   */
  Vector2& multiplyInPlace(const Vector2& otherVector);

  /**
   * @brief Returns a new Vector2 set with the multiplication of the current
   * Vector2 and the passed one coordinates.
   */
  Vector2 multiply(const Vector2& otherVector) const;

  /**
   * @brief Sets "result" coordinates with the multiplication of the current
   * Vector2 and the passed one coordinates.
   * @returns The Vector2.
   */
  const Vector2& multiplyToRef(const Vector2& otherVector,
                               Vector2& result) const;

  /**
   * @brief Returns a new Vector2 set with the Vector2 coordinates multiplied by
   * the passed floats.
   */
  Vector2 multiplyByFloats(float x, float y) const;

  /**
   * @brief Returns a new Vector2 set with the Vector2 coordinates divided by
   * the passed one coordinates.
   */
  Vector2 divide(const Vector2& otherVector) const;

  /**
   * @brief Divides in place the current Vector2 coordinates by the passed ones.
   * @returns The updated Vector2.
   */
  Vector2& divideInPlace(const Vector2& otherVector);

  /**
   * @brief Sets the "result" coordinates with the Vector2 divided by the passed
   * one coordinates.
   * @returns The Vector2.
   */
  const Vector2& divideToRef(const Vector2& otherVector, Vector2& result) const;

  /**
   * @brief Returns a new Vector2 with current Vector2 negated coordinates.
   */
  Vector2 negate() const;

  /**
   * @brief Multiply the Vector2 coordinates by scale.
   * @returns The updated Vector2.
   */
  Vector2& scaleInPlace(float scaleVal);

  /**
   * @brief Returns a new Vector2 scaled by "scale" from the current Vector2.
   */
  Vector2 scale(float scaleVal) const;

  /**
   * @brief Returns if the passed vector coordinates strictly equal the current
   * Vector2 ones.
   */
  bool equals(const Vector2& otherVector) const;

  /**
   * @brief Returns if the passed vector coordinates are close to the current
   * ones by a distance of epsilon.
   */
  bool equalsWithEpsilon(const Vector2& otherVector,
                         float epsilon = MathTools::Epsilon) const;

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
   * @brief Returns the vector length (float).
   */
  float length() const;

  /**
   * @brief Returns the vector squared length (float);
   */
  float lengthSquared() const;

  /** Methods **/

  /**
   * @brief Normalize the vector.
   * @returns The updated Vector2.
   */
  Vector2& normalize();

  /** Statics **/

  /**
   * @brief Returns a new Vector2(0, 0)
   */
  static Vector2 Zero();

  /**
   * @brief Returns a new Vector2 set from the passed index element of the
   * passed array or Float32Array.
   */
  static Vector2 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Sets "result" from the passed index element of the passed array or
   * Float32Array.
   * @returns The Vector2.
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector2& result);

  /**
   * @brief Retuns a new Vector2 located for "amount" (float) on the CatmullRom
   * spline defined by the passed four Vector2.
   */
  static Vector2 CatmullRom(const Vector2& value1, const Vector2& value2,
                            const Vector2& value3, const Vector2& value4,
                            float amount);

  /**
   * @brief Returns a new Vector2 set with same the coordinates than "value"
   * ones if the vector "value" is in the square defined by "min" and "max".
   * If a coordinate of "value" is lower than "min" coordinates, the returned
   * Vector2 is given this "min" coordinate.
   * If a coordinate of "value" is greater than "max" coordinates, the returned
   * Vector2 is given this "max" coordinate.
   */
  static Vector2 Clamp(const Vector2& value, const Vector2& min,
                       const Vector2& max);

  /**
   * @brief Returns a new Vector2 located for "amount" (float) on the Hermite
   * spline defined by the vectors "value1", "value3", "tangent1", "tangent2".
   */
  static Vector2 Hermite(const Vector2& value1, const Vector2& tangent1,
                         const Vector2& value2, const Vector2& tangent2,
                         float amount);

  /**
   * @brief Returns a new Vector2 located for "amount" (float) on the linear
   * interpolation between the vector "start" adn the vector "end".
   */
  static Vector2 Lerp(const Vector2& start, const Vector2& end, float amount);

  /**
   * @brief Returns the dot product (float) of the vector "left" and the vector
   * "right".
   */
  static float Dot(const Vector2& left, const Vector2& right);

  /**
   * @brief Returns a new Vector2 equal to the normalized passed vector.
   */
  static Vector2 Normalize(Vector2& vector);

  /**
   * @brief Returns a new Vecto2 set with the minimal coordinate values from the
   * "left" and "right" vectors.
   */
  static Vector2 Minimize(const Vector2& left, const Vector2& right);

  /**
   * @brief Returns a new Vecto2 set with the maximal coordinate values from the
   * "left" and "right" vectors.
   */
  static Vector2 Maximize(const Vector2& left, const Vector2& right);

  /**
   * @brief Returns a new Vecto2 set with the transformed coordinates of the
   * passed vector by the passed transformation matrix.
   */
  static Vector2 Transform(const Vector2& vector, const Matrix& transformation);

  /**
   * @brief Transforms the passed vector coordinates by the passed
   * transformation matrix and stores the result in the vector "result"
   * coordinates.
   */
  static void TransformToRef(const Vector2& vector,
                             const Matrix& transformation, Vector2& result);

  /**
   * @brief Retruns if the point "p" is in the triangle defined by the vertors
   * "p0", "p1", "p2"
   */
  static bool PointInTriangle(const Vector2& p, const Vector2& p0,
                              const Vector2& p1, const Vector2& p2);

  /**
   * @brief Returns the distance (float) between the vectors "value1" and
   * "value2".
   */
  static float Distance(const Vector2& value1, const Vector2& value2);

  /**
   * @brief Returns the squared distance (float) between the vectors "value1"
   * and "value2".
   */
  static float DistanceSquared(const Vector2& value1, const Vector2& value2);

  /**
   * @brief Returns a new Vecto2 located at the center of the vectors "value1"
   * and "value2".
   */
  static Vector2 Center(const Vector2& value1, const Vector2& value2);

  /**
   * @brief Returns the shortest distance (float) between the point "p" and the
   * segment defined by the two points "segA" and "segB".
   */
  static float DistanceOfPointFromSegment(const Vector2& p, const Vector2& segA,
                                          const Vector2& segB);

public:
  float x;
  float y;

}; // end of class Vector2

std::ostream& operator<<(std::ostream& os, const Vector2& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR2_H
