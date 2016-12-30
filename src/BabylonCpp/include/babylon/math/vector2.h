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
  Vector2(float x = 0.f, float y = 0.f);
  Vector2(const Vector2& otherVector);
  Vector2(Vector2&& otherVector);
  Vector2& operator=(const Vector2& otherVector);
  Vector2& operator=(Vector2&& otherVector);
  ~Vector2();
  Vector2 copy() const;
  std::unique_ptr<Vector2> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Vector2& vector);
  const char* getClassName() const;
  size_t getHashCode() const;

  /** Operators **/
  Vector2& toArray(Float32Array& array, unsigned int index = 0);
  Float32Array asArray();
  Vector2& copyFrom(const Vector2& source);
  Vector2& copyFromFloats(float ix, float iy);
  Vector2 add(const Vector2& otherVector) const;
  Vector2& addToRef(const Vector2& otherVector, Vector2& result);
  Vector2& addInPlace(const Vector2& otherVector);
  Vector2 addVector3(const Vector3& otherVector) const;
  Vector2 subtract(const Vector2& otherVector) const;
  Vector2& subtractToRef(const Vector2& otherVector, Vector2& result);
  Vector2& subtractInPlace(const Vector2& otherVector);
  Vector2& multiplyInPlace(const Vector2& otherVector);
  Vector2 multiply(const Vector2& otherVector) const;
  Vector2& multiplyToRef(const Vector2& otherVector, Vector2& result);
  Vector2 multiplyByFloats(float x, float y) const;
  Vector2 divide(const Vector2& otherVector) const;
  Vector2& divideInPlace(const Vector2& otherVector);
  Vector2& divideToRef(const Vector2& otherVector, Vector2& result);
  Vector2 negate() const;
  Vector2& scaleInPlace(float scaleVal);
  Vector2 scale(float scaleVal) const;
  bool equals(const Vector2& otherVector) const;
  bool equalsWithEpsilon(const Vector2& otherVector,
                         float epsilon = MathTools::Epsilon) const;

  /** Operator overloading **/
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
  float length() const;
  float lengthSquared() const;

  /** Methods **/
  Vector2& normalize();

  /** Statics **/
  static Vector2 Zero();
  static Vector2 FromArray(const Float32Array& array, unsigned int offset = 0);
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector2& result);
  static Vector2 CatmullRom(const Vector2& value1, const Vector2& value2,
                            const Vector2& value3, const Vector2& value4,
                            float amount);
  static Vector2 Clamp(const Vector2& value, const Vector2& min,
                       const Vector2& max);
  static Vector2 Hermite(const Vector2& value1, const Vector2& tangent1,
                         const Vector2& value2, const Vector2& tangent2,
                         float amount);
  static Vector2 Lerp(const Vector2& start, const Vector2& end, float amount);
  static float Dot(const Vector2& left, const Vector2& right);
  static Vector2 Normalize(Vector2& vector);
  static Vector2 Minimize(const Vector2& left, const Vector2& right);
  static Vector2 Maximize(const Vector2& left, const Vector2& right);
  static Vector2 Transform(const Vector2& vector, const Matrix& transformation);
  static void TransformToRef(const Vector2& vector,
                             const Matrix& transformation, Vector2& result);
  static bool PointInTriangle(const Vector2& p, const Vector2& p0,
                              const Vector2& p1, const Vector2& p2);
  static float Distance(const Vector2& value1, const Vector2& value2);
  static float DistanceSquared(const Vector2& value1, const Vector2& value2);
  static Vector2 Center(const Vector2& value1, const Vector2& value2);
  static float DistanceOfPointFromSegment(const Vector2& p, const Vector2& segA,
                                          const Vector2& segB);

public:
  float x;
  float y;

}; // end of class Vector2

std::ostream& operator<<(std::ostream& os, const Vector2& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR2_H
