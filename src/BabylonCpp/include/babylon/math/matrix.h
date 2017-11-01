#ifndef BABYLON_MATH_MATRIX_H
#define BABYLON_MATH_MATRIX_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

// SIMD
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
#include <babylon/math/simd/simd_matrix.h>
#endif

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

  /**
   * @brief Returns a new Matrix from the current Matrix.
   */
  unique_ptr_t<Matrix> clone() const;

  /**
   * @brief Returns the string "Matrix"
   */
  const char* getClassName() const;

  /**
   * @brief Returns the Matrix hash code.
   */
  int getHashCode() const;

  /**
   * @brief Mark matrix as updated.
   */
  void _markAsUpdated();

  /** Properties **/

  /**
   * @brief Returns if the matrix is the identity matrix.
   */
  bool isIdentity(bool considerAsTextureMatrix = false);

  /**
   * @brief Returns the matrix determinant (float).
   */
  float determinant() const;

  /** Methods **/

  /**
   * @brief Returns the matrix underlying array.
   */
  Float32Array toArray() const;

  /**
   * @brief Returns the matrix underlying array.
   */
  Float32Array asArray() const;

  /**
   * @brief Inverts in place the Matrix.
   * @returns The Matrix inverted.
   */
  Matrix& invert();

  /**
   * @brief Sets all the matrix elements to zero.
   * @returns The Matrix.
   */
  Matrix& reset();

  Matrix subtract(const Matrix& other) const;

  const Matrix& subtractFromRef(const Matrix& other, Matrix& result) const;

  /**
   * @brief Returns a new Matrix as the addition result of the current Matrix
   * and the passed one.
   */
  Matrix add(const Matrix& other);

  /**
   * @brief Sets the passed matrix "result" with the ddition result of the
   * current Matrix and the passed one.
   * @returns The Matrix.
   */
  Matrix& addToRef(const Matrix& other, Matrix& result);

  /**
   * @brief Adds in place the passed matrix to the current Matrix.
   * @returns The updated Matrix.
   */
  Matrix& addToSelf(const Matrix& other);

  /**
   * @brief Sets the passed matrix with the current inverted Matrix.
   * @returns The unmodified current Matrix.
   */
  Matrix& invertToRef(Matrix& other);

  /**
   * @brief Inserts the translation vector (using 3 x floats) in the current
   * Matrix.
   * @returns The updated Matrix.
   */
  Matrix& setTranslationFromFloats(float x, float y, float z);

  /**
   * @brief Inserts the translation vector in the current Matrix.
   * @returns The updated Matrix.
   */
  Matrix& setTranslation(const Vector3& vector3);

  /**
   * @brief Returns a new Vector3 as the extracted translation from the Matrix.
   */
  Vector3 getTranslation() const;

  /**
   * @brief Fill a Vector3 with the extracted translation from the Matrix.
   */
  const Matrix& getTranslationToRef(Vector3& result) const;

  /**
   * @brief Remove rotation and scaling part from the Matrix.
   * @returns The updated Matrix.
   */
  Matrix& removeRotationAndScaling();

  /**
   * @brief Returns a new Matrix set with the multiplication result of the
   * current Matrix and the passed one.
   */
  Matrix multiply(Matrix& other);

  /**
   * @brief Returns a new Matrix set with the multiplication result of the
   * current Matrix and the passed one.
   */
  Matrix multiply(Matrix&& other);

  /**
   * @brief Returns a new Matrix set with the multiplication result of the
   * current Matrix and the passed one.
   */
  Matrix& copyFrom(const Matrix& other);

  /**
   * @brief Populates the passed array from the starting index with the Matrix
   * values.
   * @returns The Matrix.
   */
  const Matrix& copyToArray(array_t<float, 16>& array,
                            unsigned int offset = 0) const;

  /**
   * @brief Populates the passed array from the starting index with the Matrix
   * values.
   * @returns The Matrix.
   */
  const Matrix& copyToArray(Float32Array& array, unsigned int offset = 0) const;

  /**
   * @brief Sets the passed matrix "result" with the multiplication result of
   * the current Matrix and the passed one.
   */
  Matrix& multiplyToRef(const Matrix& other, Matrix& result);

  /**
   * @brief Sets the Float32Array "result" from the passed index "offset" with
   * the multiplication result of the current Matrix and the passed one.
   */
  const Matrix& multiplyToArray(const Matrix& other, array_t<float, 16>& result,
                                unsigned int offset) const;

  /**
   * @brief Sets the Float32Array "result" from the passed index "offset" with
   * the multiplication result of the current Matrix and the passed one.
   */
  const Matrix& multiplyToArray(const Matrix& other, Float32Array& result,
                                unsigned int offset) const;

  /**
   * @brief Returns if the current Matrix and the passed one values are strictly
   * equal.
   */
  bool equals(const Matrix& other) const;

  /**
   * @brief Decomposes the current Matrix into :
   * - a scale vector3 passed as a reference to update,
   * - a rotation quaternion passed as a reference to update,
   * - a translation vector3 passed as a reference to update.
   * @returns The boolean `true`.
   */
  bool decompose(Vector3& scale, Quaternion& rotation,
                 Vector3& translation) const;

  /**
   * @brief Returns a new Matrix as the extracted rotation matrix from the
   * current one.
   */
  Matrix getRotationMatrix() const;

  /**
   * @brief Extracts the rotation matrix from the current one and sets it as the
   * passed "result".
   * @returns The current Matrix.
   */
  const Matrix& getRotationMatrixToRef(Matrix& result) const;

  /** Operator overloading **/
  friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix);
  Matrix operator+(const Matrix& other);
  Matrix& operator+=(const Matrix& other);
  Matrix operator-(const Matrix& other) const;
  Matrix operator*(Matrix& other);
  bool operator==(const Matrix& other) const;
  bool operator!=(const Matrix& other) const;
  float& operator[](unsigned int index);
  const float& operator[](unsigned int index) const;

  /** Statics **/

  /**
   * @brief Returns a new Matrix set from the starting index of the passed
   * array.
   */
  static Matrix FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Sets the passed "result" matrix from the starting index of the
   * passed array.
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset,
                             Matrix& result);

  /**
   * @brief Sets the passed "result" matrix from the starting index of the
   * passed Float32Array by multiplying each element by the float "scale".
   */
  static void FromFloat32ArrayToRefScaled(const Float32Array& array,
                                          unsigned int offset, float scale,
                                          Matrix& result);

  /**
   * @brief Sets the passed matrix "result" with the 16 passed floats.
   */
  static void
  FromValuesToRef(float initialM11, float initialM12, float initialM13,
                  float initialM14, float initialM21, float initialM22,
                  float initialM23, float initialM24, float initialM31,
                  float initialM32, float initialM33, float initialM34,
                  float initialM41, float initialM42, float initialM43,
                  float initialM44, Matrix& result);

  /**
   * @brief Returns the index-th row of the current matrix as a new Vector4.
   */
  Nullable<Vector4> getRow(unsigned int index) const;

  /**
   * @brief Sets the index-th row of the current matrix with the passed Vector4
   * values.
   * @returns The updated Matrix.
   */
  Matrix& setRow(unsigned int index, const Vector4& row);

  /**
   * @brief Compute the transpose of the matrix.
   * @returns a new Matrix.
   */
  Matrix transpose() const;

  /**
   * @brief Compute the transpose of the matrix.
   * @returns the current matrix.
   */
  Matrix& transposeToRef(Matrix& result);

  /**
   * @brief Sets the index-th row of the current matrix with the passed 4 x
   * float values.
   * @returns The updated Matrix.
   */
  Matrix& setRowFromFloats(unsigned int index, float x, float y, float z,
                           float w);

  /**
   * @brief Static identity matrix to be used as readonly matrix
   * Must not be updated.
   */
  static Matrix IdentityReadOnly();

  /**
   * @brief Returns a new Matrix set from the 16 passed floats.
   */
  static Matrix FromValues(float initialM11, float initialM12, float initialM13,
                           float initialM14, float initialM21, float initialM22,
                           float initialM23, float initialM24, float initialM31,
                           float initialM32, float initialM33, float initialM34,
                           float initialM41, float initialM42, float initialM43,
                           float initialM44);

  /**
   * @brief Returns a new Matrix composed by the passed scale (vector3),
   * rotation (quaternion) and translation (vector3).
   */
  static Matrix Compose(const Vector3& scale, Quaternion& rotation,
                        const Vector3& translation);

  /**
   * @brief Update a Matrix with values composed by the passed scale (vector3),
   * rotation (quaternion) and translation (vector3).
   */
  static void ComposeToRef(const Vector3& scale, Quaternion& rotation,
                           const Vector3& translation, Matrix& result);

  /**
   * @brief Returns a new indentity Matrix.
   */
  static Matrix Identity();

  /**
   * @brief Sets the passed "result" as an identity matrix.
   */
  static void IdentityToRef(Matrix& result);

  /**
   * @brief Returns a new zero Matrix.
   */
  static Matrix Zero();

  /**
   * @brief Returns a new rotation matrix for "angle" radians around the X axis.
   */
  static Matrix RotationX(float angle);

  /**
   * @brief Returns a new Matrix as the passed inverted one.
   */
  static Matrix Invert(Matrix& source);

  /**
   * @brief Sets the passed matrix "result" as a rotation matrix for "angle"
   * radians around the X axis.
   */
  static void RotationXToRef(float angle, Matrix& result);

  /**
   * @brief Returns a new rotation matrix for "angle" radians around the Y axis.
   */
  static Matrix RotationY(float angle);

  /**
   * @brief Sets the passed matrix "result" as a rotation matrix for "angle"
   * radians around the Y axis.
   */
  static void RotationYToRef(float angle, Matrix& result);

  /**
   * @brief Returns a new rotation matrix for "angle" radians around the Z axis.
   */
  static Matrix RotationZ(float angle);

  /**
   * @brief Sets the passed matrix "result" as a rotation matrix for "angle"
   * radians around the Z axis.
   */
  static void RotationZToRef(float angle, Matrix& result);

  /**
   * @brief Returns a new rotation matrix for "angle" radians around the passed
   * axis.
   */
  static Matrix RotationAxis(Vector3& axis, float angle);

  /**
   * @brief Sets the passed matrix "result" as a rotation matrix for "angle"
   * radians around the passed axis.
   */
  static void RotationAxisToRef(Vector3& axis, float angle, Matrix& result);

  /**
   * @brief Returns a new Matrix as a rotation matrix from the Euler angles (y,
   * x, z).
   */
  static Matrix RotationYawPitchRoll(float yaw, float pitch, float roll);

  /**
   * @brief Sets the passed matrix "result" as a rotation matrix from the Euler
   * angles (y, x, z).
   */
  static void RotationYawPitchRollToRef(float yaw, float pitch, float roll,
                                        Matrix& result);

  /**
   * @brief Returns a new Matrix as a scaling matrix from the passed floats (x,
   * y, z).
   */
  static Matrix Scaling(float x, float y, float z);

  /**
   * @brief Sets the passed matrix "result" as a scaling matrix from the passed
   * floats (x, y, z).
   */
  static void ScalingToRef(float x, float y, float z, Matrix& result);

  /**
   * @brief Returns a new Matrix as a translation matrix from the passed floats
   * (x, y, z).
   */
  static Matrix Translation(float x, float y, float z);

  /**
   * @brief Sets the passed matrix "result" as a translation matrix from the
   * passed floats (x, y, z).
   */
  static void TranslationToRef(float x, float y, float z, Matrix& result);

  /**
   * @brief Returns a new Matrix whose values are the interpolated values for
   * "gradien" (float) between the ones of the matrices "startValue" and
   * "endValue".
   */
  static Matrix Lerp(const Matrix& startValue, const Matrix& endValue,
                     float gradient);

  /**
   * @brief Returns a new Matrix whose values are computed by :
   * - decomposing the the "startValue" and "endValue" matrices into their
   *   respective scale, rotation and translation matrices,
   * - interpolating for "gradient" (float) the values between each of these
   *   decomposed matrices between the start and the end,
   * - recomposing a new matrix from these 3 interpolated scale, rotation and
   *   translation matrices.
   */
  static Matrix DecomposeLerp(Matrix& startValue, Matrix& endValue,
                              float gradient);

  /**
   * @brief Returns a new rotation Matrix used to rotate a mesh so as it looks
   * at the target Vector3, from the eye Vector3, the UP vector3 being
   * orientated like "up".
   * This methods works for a Left-Handed system.
   */
  static Matrix LookAtLH(const Vector3& eye, Vector3& target,
                         const Vector3& up);

  /**
   * @brief Sets the passed "result" Matrix as a rotation matrix used to rotate
   * a mesh so as it looks at the target Vector3, from the eye Vector3, the UP
   * vector3 being orientated like "up".
   * This methods works for a Left-Handed system.
   */
  static void LookAtLHToRef(const Vector3& eye, const Vector3& target,
                            const Vector3& up, Matrix& result);

  /**
   * @brief Returns a new rotation Matrix used to rotate a mesh so as it looks
   * at the target Vector3, from the eye Vector3, the UP vector3 being
   * orientated like "up".
   * This methods works for a Right-Handed system.
   */
  static Matrix LookAtRH(const Vector3& eye, Vector3& target,
                         const Vector3& up);

  /**
   * @brief Sets the passed "result" Matrix as a rotation matrix used to rotate
   * a mesh so as it looks at the target Vector3, from the eye Vector3, the UP
   * vector3 being orientated like "up".
   * This methods works for a Left-Handed system.
   */
  static void LookAtRHToRef(const Vector3& eye, const Vector3& target,
                            const Vector3& up, Matrix& result);

  /**
   * @brief Returns a new Matrix as a left-handed orthographic projection matrix
   * computed from the passed floats : width and height of the projection plane,
   * z near and far limits.
   */
  static Matrix OrthoLH(float width, float height, float znear, float zfar);

  /**
   * @brief Sets the passed matrix "result" as a left-handed orthographic
   * projection matrix computed from the passed floats : width and height of the
   * projection plane, z near and far limits.
   */
  static void OrthoLHToRef(float width, float height, float znear, float zfar,
                           Matrix& result);

  /**
   * @brief Returns a new Matrix as a left-handed orthographic projection matrix
   * computed from the passed floats : left, right, top and bottom being the
   * coordinates of the projection plane, z near and far limits.
   */
  static Matrix OrthoOffCenterLH(float left, float right, float bottom,
                                 float top, float znear, float zfar);

  /**
   * @brief Sets the passed matrix "result" as a left-handed orthographic
   * projection matrix computed from the passed floats : left, right, top and
   * bottom being the coordinates of the projection plane, z near and far
   * limits.
   */
  static void OrthoOffCenterLHToRef(float left, float right, float bottom,
                                    float top, float znear, float zfar,
                                    Matrix& result);

  /**
   * @brief Returns a new Matrix as a right-handed orthographic projection
   * matrix computed from the passed floats : left, right, top and bottom being
   * the coordinates of the projection plane, z near and far limits.
   */
  static Matrix OrthoOffCenterRH(float left, float right, float bottom,
                                 float top, float znear, float zfar);

  /**
   * @brief Sets the passed matrix "result" as a right-handed orthographic
   * projection matrix computed from the passed floats : left, right, top and
   * bottom being the coordinates of the projection plane, z near and far
   * limits.
   */
  static void OrthoOffCenterRHToRef(float left, float right, float bottom,
                                    float top, float znear, float zfar,
                                    Matrix& result);

  /**
   * @brief Returns a new Matrix as a left-handed perspective projection matrix
   * computed from the passed floats : width and height of the projection plane,
   * z near and far limits.
   */
  static Matrix PerspectiveLH(float width, float height, float znear,
                              float zfar);

  /**
   * @brief Returns a new Matrix as a left-handed perspective projection matrix
   * computed from the passed floats : vertical angle of view (fov),
   * width/height ratio (aspect), z near and far limits.
   */
  static Matrix PerspectiveFovLH(float fov, float aspect, float znear,
                                 float zfar);

  /**
   * @brief Sets the passed matrix "result" as a left-handed perspective
   * projection matrix computed from the passed floats : vertical angle of view
   * (fov), width/height ratio (aspect), z near and far limits.
   */
  static void PerspectiveFovLHToRef(float fov, float aspect, float znear,
                                    float zfar, Matrix& result,
                                    bool isVerticalFovFixed = true);

  /**
   * @brief Returns a new Matrix as a right-handed perspective projection matrix
   * computed from the passed floats : vertical angle of view (fov),
   * width/height ratio (aspect), z near and far limits.
   */
  static Matrix PerspectiveFovRH(float fov, float aspect, float znear,
                                 float zfar);

  /**
   * @brief Sets the passed matrix "result" as a right-handed perspective
   * projection matrix computed from the passed floats : vertical angle of view
   * (fov), width/height ratio (aspect), z near and far limits.
   */
  static void PerspectiveFovRHToRef(float fov, float aspect, float znear,
                                    float zfar, Matrix& result,
                                    bool isVerticalFovFixed = true);

  /**
   * @brief Sets the passed matrix "result" as a left-handed perspective
   * projection matrix  for WebVR computed from the passed floats : vertical
   * angle of view (fov), width/height ratio (aspect), z near and far limits.
   */
  static void PerspectiveFovWebVRToRef(const VRFov& fov, float znear,
                                       float zfar, Matrix& result,
                                       bool rightHanded = true);

  /**
   * @brief Returns the final transformation matrix:
   *        world * view * projection * viewport
   */
  static Matrix GetFinalMatrix(const Viewport& viewport, Matrix& world,
                               Matrix& view, Matrix& projection, float zmin,
                               float zmax);

  /**
   * @brief Returns a new Float32Array array with 4 elements : the 2x2 matrix
   * extracted from the passed Matrix.
   */
  static Float32Array GetAsMatrix2x2(const Matrix& matrix);

  /**
   * @brief Returns a new Float32Array array with 9 elements : the 3x3 matrix
   * extracted from the passed Matrix.
   */
  static Float32Array GetAsMatrix3x3(const Matrix& matrix);

  /**
   * @brief Compute the transpose of the passed Matrix.
   * @returns A new Matrix.
   */
  static Matrix Transpose(const Matrix& matrix);

  /**
   * @brief Compute the transpose of the passed Matrix and store it in the
   * result matrix.
   */
  static void TransposeToRef(const Matrix& matrix, Matrix& result);

  /**
   * @brief Returns a new Matrix as the reflection  matrix across the passed
   * plane.
   */
  static Matrix Reflection(Plane& plane);

  /**
   * @brief Sets the passed matrix "result" as the reflection matrix across the
   * passed plane.
   */
  static void ReflectionToRef(Plane& plane, Matrix& result);

  /**
   * @brief Sets the passed matrix "mat" as a rotation matrix composed from the
   * 3 passed left handed axis.
   */
  static void FromXYZAxesToRef(const Vector3& xaxis, const Vector3& yaxis,
                               const Vector3& zaxis, Matrix& result);

  /**
   * @brief Sets the passed matrix "result" as a rotation matrix according to
   * the passed quaternion.
   */
  static void FromQuaternionToRef(const Quaternion& quat, Matrix& result);

public:
  int updateFlag;
  array_t<float, 16> m;
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
  SIMD::SIMDMatrix simdMatrix;
#endif

private:
  static Quaternion _tempQuaternion;
  static Vector3 _xAxis;
  static Vector3 _yAxis;
  static Vector3 _zAxis;
  static int _updateFlagSeed;
  static Matrix _identityReadOnly;
  bool _isIdentity;
  bool _isIdentityDirty;

}; // end of class Matrix

std::ostream& operator<<(std::ostream& os, const Matrix& matrix);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATRIX_H
