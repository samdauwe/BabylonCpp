#ifndef BABYLON_MATH_MATRIX_H
#define BABYLON_MATH_MATRIX_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Matrix.
 */
class BABYLON_SHARED_EXPORT Matrix {

public:
  Matrix();
  Matrix(const Matrix& otherMatrix);
  Matrix(Matrix&& otherMatrix);
  Matrix& operator=(const Matrix& otherMatrix);
  Matrix& operator=(Matrix&& otherMatrix);
  ~Matrix();
  Matrix copy() const;
  std::unique_ptr<Matrix> clone() const;
  friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix);
  const char* getClassName() const;
  int getHashCode() const;

  /** Properties **/
  bool isIdentity() const;
  float determinant() const;

  /** Methods **/
  Float32Array toArray() const;
  Float32Array asArray() const;
  Matrix& invert();
  Matrix& reset();
  Matrix subtract(const Matrix& other);
  Matrix& subtractFromRef(const Matrix& other, Matrix& result);
  Matrix add(const Matrix& other);
  Matrix& addToRef(const Matrix& other, Matrix& result);
  Matrix& addToSelf(const Matrix& other);
  Matrix& invertToRef(Matrix& other);
  Matrix& setTranslation(const Vector3& vector3);
  Vector3 getTranslation() const;
  Matrix multiply(Matrix& other);
  Matrix multiply(Matrix&& other);
  Matrix& copyFrom(const Matrix& other);
  Matrix& copyToArray(std::array<float, 16>& array, unsigned int offset = 0);
  Matrix& copyToArray(Float32Array& array, unsigned int offset = 0);
  Matrix& multiplyToRef(const Matrix& other, Matrix& result);
  Matrix& multiplyToArray(const Matrix& other, std::array<float, 16>& result,
                          unsigned int offset);
  Matrix& multiplyToArray(const Matrix& other, Float32Array& result,
                          unsigned int offset);
  bool equals(const Matrix& other) const;
  bool decompose(Vector3& scale, Quaternion& rotation,
                 Vector3& translation) const;
  Matrix getRotationMatrix() const;
  void getRotationMatrixToRef(Matrix& result) const;

  /** Operator overloading **/
  Matrix operator+(const Matrix& other);
  Matrix& operator+=(const Matrix& other);
  Matrix operator-(const Matrix& other);
  Matrix operator*(Matrix& other);
  bool operator==(const Matrix& other) const;
  bool operator!=(const Matrix& other) const;
  float& operator[](unsigned int index);
  const float& operator[](unsigned int index) const;

  /** Statics **/
  static Matrix FromArray(const Float32Array& array, unsigned int offset = 0);
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Matrix& result);
  static void FromFloat32ArrayToRefScaled(const Float32Array& array,
                                          unsigned int offset, float scale,
                                          Matrix& result);
  Vector4 getRow(unsigned int index) const;
  Matrix& setRow(unsigned int index, const Vector4& row);
  static void
  FromValuesToRef(float initialM11, float initialM12, float initialM13,
                  float initialM14, float initialM21, float initialM22,
                  float initialM23, float initialM24, float initialM31,
                  float initialM32, float initialM33, float initialM34,
                  float initialM41, float initialM42, float initialM43,
                  float initialM44, Matrix& result);
  static Matrix FromValues(float initialM11, float initialM12, float initialM13,
                           float initialM14, float initialM21, float initialM22,
                           float initialM23, float initialM24, float initialM31,
                           float initialM32, float initialM33, float initialM34,
                           float initialM41, float initialM42, float initialM43,
                           float initialM44);
  static Matrix Compose(const Vector3& scale, Quaternion& rotation,
                        const Vector3& translation);
  static Matrix Identity();
  static void IdentityToRef(Matrix& result);
  static Matrix Zero();
  static Matrix RotationX(float angle);
  static Matrix Invert(Matrix& source);
  static void RotationXToRef(float angle, Matrix& result);
  static Matrix RotationY(float angle);
  static void RotationYToRef(float angle, Matrix& result);
  static Matrix RotationZ(float angle);
  static void RotationZToRef(float angle, Matrix& result);
  static Matrix RotationAxis(Vector3& axis, float angle);
  static void RotationAxisToRef(Vector3& axis, float angle, Matrix& result);
  static Matrix RotationYawPitchRoll(float yaw, float pitch, float roll);
  static void RotationYawPitchRollToRef(float yaw, float pitch, float roll,
                                        Matrix& result);
  static Matrix Scaling(float x, float y, float z);
  static void ScalingToRef(float x, float y, float z, Matrix& result);
  static Matrix Translation(float x, float y, float z);
  static void TranslationToRef(float x, float y, float z, Matrix& result);
  static Matrix Lerp(const Matrix& startValue, const Matrix& endValue,
                     float gradient);
  static Matrix DecomposeLerp(Matrix& startValue, Matrix& endValue,
                              float gradient);
  static Matrix LookAtLH(const Vector3& eye, Vector3& target,
                         const Vector3& up);
  static void LookAtLHToRef(const Vector3& eye, const Vector3& target,
                            const Vector3& up, Matrix& result);
  static Matrix LookAtRH(const Vector3& eye, Vector3& target,
                         const Vector3& up);
  static void LookAtRHToRef(const Vector3& eye, const Vector3& target,
                            const Vector3& up, Matrix& result);
  static Matrix OrthoLH(float width, float height, float znear, float zfar);
  static void OrthoLHToRef(float width, float height, float znear, float zfar,
                           Matrix& result);
  static Matrix OrthoOffCenterLH(float left, float right, float bottom,
                                 float top, float znear, float zfar);
  static void OrthoOffCenterLHToRef(float left, float right, float bottom,
                                    float top, float znear, float zfar,
                                    Matrix& result);
  static Matrix OrthoOffCenterRH(float left, float right, float bottom,
                                 float top, float znear, float zfar);
  static void OrthoOffCenterRHToRef(float left, float right, float bottom,
                                    float top, float znear, float zfar,
                                    Matrix& result);
  static Matrix PerspectiveLH(float width, float height, float znear,
                              float zfar);
  static Matrix PerspectiveFovLH(float fov, float aspect, float znear,
                                 float zfar);
  static void PerspectiveFovLHToRef(float fov, float aspect, float znear,
                                    float zfar, Matrix& result,
                                    bool isVerticalFovFixed = true);
  static Matrix PerspectiveFovRH(float fov, float aspect, float znear,
                                 float zfar);
  static void PerspectiveFovRHToRef(float fov, float aspect, float znear,
                                    float zfar, Matrix& result,
                                    bool isVerticalFovFixed = true);
  static void PerspectiveFovWebVRToRef(const VRFov& fov, float znear,
                                       float zfar, Matrix& result,
                                       bool isVerticalFovFixed = true);
  static Matrix GetFinalMatrix(const Viewport& viewport, Matrix& world,
                               Matrix& view, Matrix& projection, float zmin,
                               float zmax);
  static Float32Array GetAsMatrix2x2(const Matrix& matrix);
  static Float32Array GetAsMatrix3x3(const Matrix& matrix);
  static Matrix Transpose(const Matrix& matrix);
  static Matrix Reflection(Plane& plane);
  static void ReflectionToRef(Plane& plane, Matrix& result);
  static void FromXYZAxesToRef(const Vector3& xaxis, const Vector3& yaxis,
                               const Vector3& zaxis, Matrix& mat);
  static void FromQuaternionToRef(const Quaternion& quat, Matrix& result);

public:
  std::array<float, 16> m;

}; // end of class Matrix

std::ostream& operator<<(std::ostream& os, const Matrix& matrix);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATRIX_H
