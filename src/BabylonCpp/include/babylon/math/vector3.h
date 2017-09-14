#ifndef BABYLON_MATH_VECTOR3_H
#define BABYLON_MATH_VECTOR3_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Vector3.
 */
class BABYLON_SHARED_EXPORT Vector3 {

public:
  /**
   * @brief Creates a new Vector3 object from the passed x, y, z (floats)
   * coordinates.
   * A Vector3 is the main object used in 3D geometry.
   * It can represent etiher the coordinates of a point the space, either a
   * direction.
   */
  Vector3(float x = 0.f, float y = 0.f, float z = 0.f);
  Vector3(const Vector3& otherVector);
  Vector3(Vector3&& otherVector);
  ~Vector3();
  Vector3& operator=(const Vector3& otherVector);
  Vector3& operator=(Vector3&& otherVector);
  Vector3 copy() const;

  /**
   * @brief Returns a new Vector3 copied from the current Vector3.
   */
  std::unique_ptr<Vector3> clone() const;

  /**
   * @brief Returns a string with the Vector3 coordinates.
   */
  string_t toString() const;

  /**
   * @brief Returns the string "Vector3".
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Vector hash code.
   */
  size_t getHashCode() const;

  /** Operators **/

  /**
   * @brief Returns a new array with three elements: the coordinates the
   * Vector3.
   */
  Float32Array asArray() const;

  /**
   * @brief Populates the passed array or Float32Array from the passed index
   * with the successive coordinates of the Vector3.
   * @returns The Vector3.
   */
  const Vector3& toArray(Float32Array& array, unsigned int index = 0) const;

  /**
   * @brief Returns a new Quaternion object, computed from the Vector3
   * coordinates.
   */
  Quaternion toQuaternion() const;

  /**
   * @brief Adds the passed vector to the current Vector3.
   * @returns The updated Vector3.
   */
  Vector3& addInPlace(const Vector3& otherVector);

  /**
   * @brief Returns a new Vector3, result of the addition the current Vector3
   * and the passed vector.
   */
  Vector3 add(const Vector3& otherVector) const;

  /**
   * @brief Adds the current Vector3 to the passed one and stores the result in
   * the vector "result".
   * @returns the current Vector3.
   */
  const Vector3& addToRef(const Vector3& otherVector, Vector3& result) const;

  /**
   * @brief Subtract the passed vector from the current Vector3.
   * @returns The updated Vector3.
   */
  Vector3& subtractInPlace(const Vector3& otherVector);

  /**
   * @brief Returns a new Vector3, result of the subtraction of the passed
   * vector from the current Vector3.
   */
  Vector3 subtract(const Vector3& otherVector) const;

  /**
   * @brief Subtracts the passed vector from the current Vector3 and stores the
   * result in the vector "result".
   * @returns The current Vector3.
   */
  const Vector3& subtractToRef(const Vector3& otherVector,
                               Vector3& result) const;

  /**
   * @brief Returns a new Vector3 set with the subtraction of the passed floats
   * from the current Vector3 coordinates.
   */
  Vector3 subtractFromFloats(float x, float y, float z) const;

  /**
   * @brief Subtracts the passed floats from the current Vector3 coordinates and
   * set the passed vector "result" with this result.
   * @returns The current Vector3.
   */
  const Vector3& subtractFromFloatsToRef(float x, float y, float z,
                                         Vector3& result) const;

  /**
   * @brief Returns a new Vector3 set with the current Vector3 negated
   * coordinates.
   */
  Vector3 negate() const;

  /**
   * @brief Multiplies the Vector3 coordinates by the float "scale".
   * @returns The updated Vector3.
   */
  Vector3& scaleInPlace(float iscale);

  /**
   * @brief Returns a new Vector3 set with the current Vector3 coordinates
   * multiplied by the float "scale".
   */
  Vector3 scale(float iscale) const;

  /**
   * @brief Multiplies the current Vector3 coordinates by the float "scale" and
   * stores the result in the passed vector "result" coordinates.
   * @returns the current Vector3.
   */
  const Vector3& scaleToRef(int iscale, Vector3& result) const;
  const Vector3& scaleToRef(float iscale, Vector3& result) const;

  /**
   * @brief Returns if the current Vector3 and the passed vector coordinates are
   * strictly equal.
   */
  bool equals(const Vector3& otherVector) const;

  /**
   * @brief Returns if the current Vector3 and the passed vector coordinates are
   * distant less than epsilon.
   */
  bool equalsWithEpsilon(const Vector3& otherVector,
                         float epsilon = Math::Epsilon) const;

  /**
   * @brief Returns if the current Vector3 coordinate equal the passed floats.
   */
  bool equalsToFloats(float x, float y, float z) const;

  /**
   * @brief Muliplies the current Vector3 coordinates by the passed ones.
   * @returns The updated Vector3.
   */
  Vector3& multiplyInPlace(const Vector3& otherVector);

  /**
   * @brief Returns a new Vector3, result of the multiplication of the current
   * Vector3 by the passed vector.
   */
  Vector3 multiply(const Vector3& otherVector) const;

  /**
   * @brief Multiplies the current Vector3 by the passed one and stores the
   * result in the passed vector "result".
   * @returns The current Vector3.
   */
  const Vector3& multiplyToRef(const Vector3& otherVector,
                               Vector3& result) const;

  /**
   * @brief Returns a new Vector3 set with the result of the mulliplication of
   * the current Vector3 coordinates by the passed floats.
   */
  Vector3 multiplyByFloats(float x, float y, float z) const;

  /**
   * @brief Divides in place the current Vector3 coordinates by the passed ones.
   * @returns The updated Vector3.
   */
  Vector3& divideInPlace(const Vector3& otherVector);

  /**
   * @brief Returns a new Vector3 set witth the result of the division of the
   * current Vector3 coordinates by the passed ones.
   */
  Vector3 divide(const Vector3& otherVector) const;

  /**
   * @brief Divides the current Vector3 coordinates by the passed ones and
   * stores the result in the passed vector "result".
   * @returns The current Vector3.
   */
  const Vector3& divideToRef(const Vector3& otherVector, Vector3& result) const;

  /**
   * @brief Updates the current Vector3 with the minimal coordinate values
   * between its and the passed vector ones.
   * @returns The updated Vector3.
   */
  Vector3& minimizeInPlace(const Vector3& other);

  /**
   * @brief Updates the current Vector3 with the maximal coordinate values
   * between its and the passed vector ones.
   * @returns The updated Vector3.
   */
  Vector3& maximizeInPlace(const Vector3& other);

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
  Vector3 operator/(const Vector3& otherVector) const;
  Vector3& operator/=(float scaleVal);
  Vector3& operator/=(const Vector3& otherVector);
  bool operator==(const Vector3& otherVector) const;
  bool operator!=(const Vector3& otherVector) const;
  const float& operator[](const unsigned int index) const;

  /** Properties **/

  /**
   * @brief Returns the length of the Vector3 (float).
   */
  float length() const;

  /**
   * @brief Returns the squared length of the Vector3 (float).
   */
  float lengthSquared() const;

  /** Methods **/

  /**
   * @brief Normalize the current Vector3.
   * @returns The updated Vector3.
   */
  Vector3& normalize();

  /**
   * @brief Copies the passed vector coordinates to the current Vector3 ones.
   * @returns The updated Vector3.
   */
  Vector3& copyFrom(const Vector3& source);

  /**
   * @brief Copies the passed floats to the current Vector3 coordinates.
   * @returns The updated Vector3.
   */
  Vector3& copyFromFloats(float x, float y, float z);

  /**
   * @brief Copies the passed floats to the current Vector3 coordinates.
   * @returns The updated Vector3.
   */
  Vector3& set(float x, float y, float z);

  /** Statics **/
  static float GetClipFactor(const Vector3& vector0, const Vector3& vector1,
                             const Vector3& axis, float size);

  /**
   * @brief Returns a new Vector3 set from the index "offset" of the passed
   * array or Float32Array.
   */
  static Vector3 FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Sets the passed vector "result" with the element values from the
   * index "offset" of the passed array or Float32Array.
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector3& result);

  /**
   * @brief Sets the passed vector "result" with the element values from the
   * index "offset" of the passed array or float array.
   */
  static void FromArrayToRef(const array_t<float, 16>& array,
                             unsigned int offset, Vector3& result);

  /**
   * @brief Sets the passed vector "result" with the passed floats.
   */
  static void FromFloatsToRef(float ix, float iy, float iz, Vector3& result);

  /**
   * @brief Returns a new Vector3 set to (0.0, 0.0, 0.0).
   */
  static Vector3 Zero();

  /**
   * @brief Returns a new Vector3 set to (1.0, 1.0, 1.0).
   */
  static Vector3 One();

  /**
   * @brief Returns a new Vector3 set to (0.0, 1.0, 0.0).
   */
  static Vector3 Up();

  /**
   * @brief Returns a new Vector3 set to (0.0, 0.0, 1.0).
   */
  static Vector3 Forward();

  /**
   * @brief Returns a new Vector3 set to (1.0, 0.0, 0.0).
   */
  static Vector3 Right();

  /**
   * @brief Returns a new Vector3 set to (-1.0, 0.0, 0.0).
   */
  static Vector3 Left();

  /**
   * @brief Returns a new Vector3 set with the result of the transformation by
   * the passed matrix of the passed vector.
   * This method computes tranformed coordinates only, not transformed direction
   * vectors.
   */
  static Vector3 TransformCoordinates(const Vector3& vector,
                                      const Matrix& transformation);

  /**
   * @brief Sets the passed vector "result" coordinates with the result of the
   * transformation by the passed matrix of the passed vector.
   * This method computes tranformed coordinates only, not transformed direction
   * vectors.
   */
  static void TransformCoordinatesToRef(const Vector3& vector,
                                        const Matrix& transformation,
                                        Vector3& result);

  /**
   * @brief Sets the passed vector "result" coordinates with the result of the
   * transformation by the passed matrix of the passed floats (x, y, z).
   * This method computes tranformed coordinates only, not transformed direction
   * vectors.
   */
  static void TransformCoordinatesFromFloatsToRef(float x, float y, float z,
                                                  const Matrix& transformation,
                                                  Vector3& result);

  /**
   * @brief Returns a new Vector3 set with the result of the normal
   * transformation by the passed matrix of the passed vector.
   * This methods computes transformed normalized direction vectors only.
   */
  static Vector3 TransformNormal(const Vector3& vector,
                                 const Matrix& transformation);

  /**
   * @brief Sets the passed vector "result" with the result of the normal
   * transformation by the passed matrix of the passed vector.
   * This methods computes transformed normalized direction vectors only.
   */
  static void TransformNormalToRef(const Vector3& vector,
                                   const Matrix& transformation,
                                   Vector3& result);

  /**
   * @brief Sets the passed vector "result" with the result of the normal
   * transformation by the passed matrix of the passed floats (x, y, z).
   * This methods computes transformed normalized direction vectors only.
   */
  static void TransformNormalFromFloatsToRef(float x, float y, float z,
                                             const Matrix& transformation,
                                             Vector3& result);

  /**
   * @brief Returns a new Vector3 located for "amount" on the CatmullRom
   * interpolation spline defined by the vectors "value1", "value2", "value3",
   * "value4".
   */
  static Vector3 CatmullRom(const Vector3& value1, const Vector3& value2,
                            const Vector3& value3, const Vector3& value4,
                            float amount);

  /**
   * @brief Returns a new Vector3 set with the coordinates of "value", if the
   * vector "value" is in the cube defined by the vectors "min" and "max".
   * If a coordinate value of "value" is lower than one of the "min" coordinate,
   * then this "value" coordinate is set with the "min" one.
   * If a coordinate value of "value" is greater than one of the "max"
   * coordinate, then this "value" coordinate is set with the "max" one.
   */
  static Vector3 Clamp(const Vector3& value, const Vector3& min,
                       const Vector3& max);

  /**
   * @brief Returns a new Vector3 located for "amount" (float) on the Hermite
   * interpolation spline defined by the vectors "value1", "tangent1", "value2",
   * "tangent2".
   */
  static Vector3 Hermite(const Vector3& value1, const Vector3& tangent1,
                         const Vector3& value2, const Vector3& tangent2,
                         float amount);

  /**
   * @brief Returns a new Vector3 located for "amount" (float) on the linear
   * interpolation between the vectors "start" and "end".
   */
  static Vector3 Lerp(const Vector3& start, const Vector3& end, float amount);

  /**
   * @brief Sets the passed vector "result" with the result of the linear
   * interpolation from the vector "start" for "amount" to the vector "end".
   */
  static void LerpToRef(const Vector3& start, const Vector3& end, float amount,
                        Vector3& result);

  /**
   * @brief Returns the dot product (float) between the vectors "left" and
   * "right".
   */
  static float Dot(const Vector3& left, const Vector3& right);

  /**
   * @brief Returns a new Vector3 as the cross product of the vectors "left" and
   * "right".
   * The cross product is then orthogonal to both "left" and "right".
   */
  static Vector3 Cross(const Vector3& left, const Vector3& right);

  /**
   * @brief Sets the passed vector "result" with the cross product of "left" and
   * "right".
   * The cross product is then orthogonal to both "left" and "right".
   */
  static void CrossToRef(const Vector3& left, const Vector3& right,
                         Vector3& result);

  /**
   * @brief Returns a new Vector3 as the normalization of the passed vector.
   */
  static Vector3 Normalize(const Vector3& vector);

  /**
   * @brief Sets the passed vector "result" with the normalization of the passed
   * first vector.
   */
  static void NormalizeToRef(const Vector3& vector, Vector3& result);

  static Vector3 Project(const Vector3& vector, Matrix& world,
                         Matrix& transform, const Viewport& viewport);

  static Vector3 UnprojectFromTransform(Vector3& source, float viewportWidth,
                                        float viewportHeight, Matrix& world,
                                        Matrix& transform);

  static Vector3 Unproject(const Vector3& source, float viewportWidth,
                           float viewportHeight, Matrix& world, Matrix& view,
                           Matrix& projection);

  static Vector3 Minimize(const Vector3& left, const Vector3& right);

  static Vector3 Maximize(const Vector3& left, const Vector3& right);

  /**
   * Returns the distance (float) between the vectors "value1" and "value2".
   */
  static float Distance(const Vector3& value1, const Vector3& value2);

  /**
   * @brief Returns the squared distance (float) between the vectors "value1"
   * and "value2".
   */
  static float DistanceSquared(const Vector3& value1, const Vector3& value2);

  /**
   * @brief Returns a new Vector3 located at the center between "value1" and
   * "value2".
   */
  static Vector3 Center(const Vector3& value1, const Vector3& value2);

  /**
   * @brief Given three orthogonal normalized left-handed oriented Vector3 axis
   * in space (target system), RotationFromAxis() returns the rotation Euler
   * angles (ex : rotation.x, rotation.y, rotation.z) to apply to something in
   * order to rotate it from its local system to the given target system.
   * Note: axis1, axis2 and axis3 are normalized during this operation.
   * @returns A new Vector3.
   */
  static Vector3 RotationFromAxis(Vector3& axis1, Vector3& axis2,
                                  Vector3& axis3);

  /**
   * @brief The same than RotationFromAxis but updates the passed ref Vector3
   * parameter instead of returning a new Vector3.
   */
  static void RotationFromAxisToRef(Vector3& axis1, Vector3& axis2,
                                    Vector3& axis3, Vector3& ref);

public:
  float x;
  float y;
  float z;

}; // end of class Vector3

std::ostream& operator<<(std::ostream& os, const Vector3& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR3_H
