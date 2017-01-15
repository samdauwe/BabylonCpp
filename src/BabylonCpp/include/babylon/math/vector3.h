#ifndef BABYLON_MATH_VECTOR3_H
#define BABYLON_MATH_VECTOR3_H

#include <babylon/babylon_global.h>
#include <babylon/math/math_tools.h>

namespace BABYLON {

/**
 * @brief Represents a Vector3.
 */
class BABYLON_SHARED_EXPORT Vector3 {

public:
  Vector3(float x = 0.f, float y = 0.f, float z = 0.f);
  Vector3(const Vector3& otherVector);
  Vector3(Vector3&& otherVector);
  ~Vector3();
  Vector3& operator=(const Vector3& otherVector);
  Vector3& operator=(Vector3&& otherVector);
  Vector3 copy() const;
  std::unique_ptr<Vector3> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
  const char* getClassName() const;
  size_t getHashCode() const;

  /** Operators **/
  Float32Array asArray() const;
  Vector3& toArray(Float32Array& array, unsigned int index = 0);
  Quaternion toQuaternion() const;
  Vector3& addInPlace(const Vector3& otherVector);
  Vector3 add(const Vector3& otherVector) const;
  Vector3& addToRef(const Vector3& otherVector, Vector3& result);
  Vector3& subtractInPlace(const Vector3& otherVector);
  Vector3 subtract(const Vector3& otherVector) const;
  const Vector3& subtractToRef(const Vector3& otherVector,
                               Vector3& result) const;
  Vector3 subtractFromFloats(float ix, float iy, float iz) const;
  Vector3& subtractFromFloatsToRef(float ix, float iy, float iz,
                                   Vector3& result);
  Vector3 negate() const;
  Vector3& scaleInPlace(float iscale);
  Vector3 scale(float iscale) const;
  void scaleToRef(int iscale, Vector3& result);
  void scaleToRef(float iscale, Vector3& result);
  bool equals(const Vector3& otherVector) const;
  bool equalsWithEpsilon(const Vector3& otherVector,
                         float epsilon = MathTools::Epsilon) const;
  bool equalsToFloats(float ix, float iy, float iz) const;
  Vector3& multiplyInPlace(const Vector3& otherVector);
  Vector3 multiply(const Vector3& otherVector) const;
  Vector3& multiplyToRef(const Vector3& otherVector, Vector3& result);
  Vector3 multiplyByFloats(float ix, float iy, float iz) const;
  Vector3& divideInPlace(const Vector3& otherVector);
  Vector3 divide(const Vector3& otherVector) const;
  Vector3& divideToRef(const Vector3& otherVector, Vector3& result);
  Vector3& minimizeInPlace(const Vector3& other);
  Vector3& maximizeInPlace(const Vector3& other);

  /** Operator overloading **/
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
  Vector3& operator/=(const Vector3& otherVector);
  bool operator==(const Vector3& otherVector) const;
  bool operator!=(const Vector3& otherVector) const;

  /** Properties **/
  float length() const;
  float lengthSquared() const;

  /** Methods **/
  Vector3& normalize();
  Vector3& copyFrom(const Vector3& source);
  Vector3& copyFromFloats(float x, float y, float z);

  /** Statics **/
  static float GetClipFactor(const Vector3& vector0, const Vector3& vector1,
                             const Vector3& axis, float size);
  static Vector3 FromArray(const Float32Array& array, unsigned int offset = 0);
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Vector3& result);
  static void FromArrayToRef(const std::array<float, 16>& array,
                             unsigned int offset, Vector3& result);
  static void FromFloatsToRef(float ix, float iy, float iz, Vector3& result);
  static Vector3 Zero();
  static Vector3 Up();
  static Vector3 Forward();
  static Vector3 Right();
  static Vector3 Left();
  static Vector3 TransformCoordinates(const Vector3& vector,
                                      const Matrix& transformation);
  static void TransformCoordinatesToRef(const Vector3& vector,
                                        const Matrix& transformation,
                                        Vector3& result);
  static void TransformCoordinatesFromFloatsToRef(float x, float y, float z,
                                                  const Matrix& transformation,
                                                  Vector3& result);
  static Vector3 TransformNormal(const Vector3& vector,
                                 const Matrix& transformation);
  static void TransformNormalToRef(const Vector3& vector,
                                   const Matrix& transformation,
                                   Vector3& result);
  static void TransformNormalFromFloatsToRef(float x, float y, float z,
                                             const Matrix& transformation,
                                             Vector3& result);
  static Vector3 CatmullRom(const Vector3& value1, const Vector3& value2,
                            const Vector3& value3, const Vector3& value4,
                            float amount);
  static Vector3 Clamp(const Vector3& value, const Vector3& min,
                       const Vector3& max);
  static Vector3 Hermite(const Vector3& value1, const Vector3& tangent1,
                         const Vector3& value2, const Vector3& tangent2,
                         float amount);
  static Vector3 Lerp(const Vector3& start, const Vector3& end, float amount);
  static float Dot(const Vector3& left, const Vector3& right);
  static Vector3 Cross(const Vector3& left, const Vector3& right);
  static void CrossToRef(const Vector3& left, const Vector3& right,
                         Vector3& result);
  static Vector3 Normalize(const Vector3& vector);
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
  static float Distance(const Vector3& value1, const Vector3& value2);
  static float DistanceSquared(const Vector3& value1, const Vector3& value2);
  static Vector3 Center(const Vector3& value1, const Vector3& value2);
  static Vector3 RotationFromAxis(const Vector3& axis1, const Vector3& axis2,
                                  const Vector3& axis3);
  static void RotationFromAxisToRef(const Vector3& axis1, const Vector3& axis2,
                                    const Vector3& axis3, Vector3& ref);

public:
  float x;
  float y;
  float z;

}; // end of class Vector3

std::ostream& operator<<(std::ostream& os, const Vector3& vector);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_VECTOR3_H
