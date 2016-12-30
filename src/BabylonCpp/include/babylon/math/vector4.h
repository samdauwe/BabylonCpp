#ifndef BABYLON_MATH_VECTOR4_H
#define BABYLON_MATH_VECTOR4_H

#include <babylon/babylon_global.h>
#include <babylon/math/math_tools.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Vector4 class created for EulerAngle class conversion to Quaternion.
 */
class BABYLON_SHARED_EXPORT Vector4 {

public:
  Vector4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f);
  Vector4(const Vector4& otherVector);
  Vector4(Vector4&& otherVector);
  Vector4& operator=(const Vector4& otherVector);
  Vector4& operator=(Vector4&& otherVector);
  ~Vector4();
  Vector4 copy() const;
  std::unique_ptr<Vector4> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Vector4& vector);
  const char* getClassName() const;
  size_t getHashCode() const;

  /** Operators **/
  Float32Array asArray() const;
  Vector4& toArray(Float32Array& array, unsigned int index = 0);
  Vector4& addInPlace(const Vector4& otherVector);
  Vector4 add(const Vector4& otherVector) const;
  Vector4& addToRef(const Vector4& otherVector, Vector4& result);
  Vector4& subtractInPlace(const Vector4& otherVector);
  Vector4 subtract(const Vector4& otherVector) const;
  Vector4& subtractToRef(const Vector4& otherVector, Vector4& result);
  Vector4 subtractFromFloats(float ix, float iy, float iz, float iw) const;
  Vector4& subtractFromFloatsToRef(float ix, float iy, float iz, float iw,
                                   Vector4& result);
  Vector4 negate() const;
  Vector4& scaleInPlace(float iscale);
  Vector4 scale(float iscale) const;
  void scaleToRef(float iscale, Vector4& result);
  bool equals(const Vector4& otherVector) const;
  bool equalsWithEpsilon(const Vector4& otherVector,
                         float epsilon = MathTools::Epsilon) const;
  bool equalsToFloats(float ix, float iy, float iz, float iw) const;
  Vector4& multiplyInPlace(const Vector4& otherVector);
  Vector4 multiply(const Vector4& otherVector) const;
  Vector4& multiplyToRef(const Vector4& otherVector, Vector4& result);
  Vector4 multiplyByFloats(float ix, float iy, float iz, float iw) const;
  Vector4 divide(const Vector4& otherVector) const;
  Vector4& divideToRef(const Vector4& otherVector, Vector4& result);
  Vector4& minimizeInPlace(const Vector4& other);
  Vector4& maximizeInPlace(const Vector4& other);

  /** Operator overloading **/
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
  float length() const;
  float lengthSquared() const;

  /** Methods **/
  Vector4& normalize();
  Vector3 toVector3() const;
  Vector4& copyFrom(const Vector4& source);
  Vector4& copyFromFloats(float ix, float iy, float iz, float iw);

  /** Statics **/
  static Vector4 FromArray(const Float32Array& array, unsigned int offset = 0);
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector4& result);
  static void FromFloatsToRef(float ix, float iy, float iz, float iw,
                              Vector4& result);
  static Vector4 Zero();
  static Vector4 Normalize(const Vector4& vector);
  static void NormalizeToRef(const Vector4& vector, Vector4& result);
  static Vector4 Minimize(const Vector4& left, const Vector4& right);
  static Vector4 Maximize(const Vector4& left, const Vector4& right);
  static float Distance(const Vector4& value1, const Vector4& value2);
  static float DistanceSquared(const Vector4& value1, const Vector4& value2);
  static Vector4 Center(const Vector4& value1, const Vector4& value2);

public:
  float x;
  float y;
  float z;
  float w;

}; // end of class Vector4

std::ostream& operator<<(std::ostream& os, const Vector4& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR4_H
