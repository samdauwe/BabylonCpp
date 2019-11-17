#ifndef BABYLON_MATH_MATRIX_H
#define BABYLON_MATH_MATRIX_H

#include <array>
#include <memory>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

// SIMD
#if BABYLONCPP_OPTION_ENABLE_SIMD == true
#include <babylon/math/simd/simd_matrix.h>
#endif

namespace BABYLON {

class Plane;
class Quaternion;
class Vector3;
class Vector4;
class Viewport;
struct VRFov;

/**
 * @brief Class used to store matrix data (4x4).
 */
class BABYLON_SHARED_EXPORT Matrix {

public:
  /**
   * @brief Creates an empty matrix (filled with zeros).
   */
  Matrix();
  Matrix(const Matrix& otherMatrix);
  Matrix(Matrix&& otherMatrix);
  Matrix& operator=(const Matrix& otherMatrix);
  Matrix& operator=(Matrix&& otherMatrix);
  ~Matrix(); // = default
  [[nodiscard]] Matrix copy() const;

  /**
   * @brief Clone the current matrix.
   * @returns a new matrix from the current matrix
   */
  [[nodiscard]] std::unique_ptr<Matrix> clone() const;

  /**
   * @brief Returns the name of the current matrix class.
   * @returns the string "Matrix"
   */
  [[nodiscard]] const char* getClassName() const;

  /**
   * @brief Returns the Matrix hash code.
   */
  [[nodiscard]] int getHashCode() const;

  /**
   * @brief Gets the internal data of the matrix.
   */
  [[nodiscard]] const std::array<float, 16>& m() const;

  /**
   * @brief Mark matrix as updated.
   */
  void _markAsUpdated();

  /** Properties **/

  /**
   * @brief Check if the current matrix is identity.
   * @returns true is the matrix is the identity matrix
   */
  bool isIdentity();

  /**
   * @brief Check if the current matrix is identity as a texture matrix (3x2
   * store in 4x4).
   * @returns true is the matrix is the identity matrix
   */
  bool isIdentityAs3x2();

  /**
   * @brief Gets the determinant of the matrix.
   * @returns the matrix determinant
   */
  [[nodiscard]] float determinant() const;

  /** Methods **/

  /**
   * @brief Returns the matrix as a Float32Array.
   * @returns the matrix underlying array
   */
  [[nodiscard]] Float32Array toArray() const;

  /**
   * @brief Returns the matrix as a Float32Array.
   * @returns the matrix underlying array.
   */
  [[nodiscard]] Float32Array asArray() const;

  /**
   * @brief Inverts the current matrix in place.
   * @returns the current inverted matrix
   */
  Matrix& invert();

  /**
   * @brief Sets all the matrix elements to zero.
   * @returns the current matrix
   */
  Matrix& reset();

  [[nodiscard]] Matrix subtract(const Matrix& other) const;

  const Matrix& subtractFromRef(const Matrix& other, Matrix& result) const;

  /**
   * @brief Adds the current matrix with a second one.
   * @param other defines the matrix to add
   * @returns a new matrix as the addition of the current matrix and the given
   * one
   */
  Matrix add(const Matrix& other);

  /**
   * @brief Sets the given matrix "result" to the addition of the current matrix
   * and the given one.
   * @param other defines the matrix to add
   * @param result defines the target matrix
   * @returns the current matrix
   */
  Matrix& addToRef(const Matrix& other, Matrix& result);

  /**
   * @brief Adds in place the given matrix to the current matrix.
   * @param other defines the second operand
   * @returns the current updated matrix
   */
  Matrix& addToSelf(const Matrix& other);

  /**
   * @brief Sets the given matrix to the current inverted Matrix.
   * @param other defines the target matrix
   * @returns the unmodified current matrix
   */
  Matrix& invertToRef(Matrix& other);

  /**
   * @brief Add a value at the specified position in the current Matrix.
   * @param index the index of the value within the matrix. between 0 and 15.
   * @param value the value to be added
   * @returns the current updated matrix
   */
  Matrix& addAtIndex(size_t index, float value);

  /**
   * @brief Mutiply the specified position in the current Matrix by a value.
   * @param index the index of the value within the matrix. between 0 and 15.
   * @param value the value to be added
   * @returns the current updated matrix
   */
  Matrix& multiplyAtIndex(size_t index, float value);

  /**
   * @brief Sets a value at the specified position in the current Matrix.
   * @param index the index of the value within the matrix. between 0 and 15.
   * @param value the value to be added
   * @returns the current updated matrix
   */
  Matrix& setAtIndex(size_t index, float value);

  /**
   * @brief Inserts the translation vector (using 3 floats) in the current
   * matrix.
   * @param x defines the 1st component of the translation
   * @param y defines the 2nd component of the translation
   * @param z defines the 3rd component of the translation
   * @returns the current updated matrix
   */
  Matrix& setTranslationFromFloats(float x, float y, float z);

  /**
   * @brief Adds the translation vector (using 3 floats) in the current matrix.
   * @param x defines the 1st component of the translation
   * @param y defines the 2nd component of the translation
   * @param z defines the 3rd component of the translation
   * @returns the current updated matrix
   */
  Matrix& addTranslationFromFloats(float x, float y, float z);

  /**
   * @brief Inserts the translation vector in the current matrix.
   * @param vector3 defines the translation to insert
   * @returns the current updated matrix
   */
  Matrix& setTranslation(const Vector3& vector3);

  /**
   * @brief Gets the translation value of the current matrix.
   * @returns a new Vector3 as the extracted translation from the matrix
   */
  [[nodiscard]] Vector3 getTranslation() const;

  /**
   * @brief Fill a Vector3 with the extracted translation from the matrix.
   * @param result defines the Vector3 where to store the translation
   * @returns the current matrix
   */
  const Matrix& getTranslationToRef(Vector3& result) const;

  /**
   * @brief Remove rotation and scaling part from the matrix.
   * @returns the updated matrix
   */
  Matrix& removeRotationAndScaling();

  /**
   * @brief Multiply two matrices.
   * @param other defines the second operand
   * @returns a new matrix set with the multiplication result of the current
   * Matrix and the given one
   */
  Matrix multiply(Matrix& other);

  /**
   * @brief Multiply two matrices.
   * @param other defines the second operand
   * @returns a new matrix set with the multiplication result of the current
   * Matrix and the given one
   */
  Matrix multiply(Matrix&& other);

  /**
   * @brief Copy the current matrix from the given one.
   * @param other defines the source matrix
   * @returns the current updated matrix
   */
  Matrix& copyFrom(const Matrix& other);

  /**
   * @brief Populates the given array from the starting index with the current
   * matrix values.
   * @param array defines the target array
   * @param offset defines the offset in the target array where to start storing
   * values
   * @returns the current matrix
   */
  const Matrix& copyToArray(std::array<float, 16>& array, unsigned int offset = 0) const;

  /**
   * @brief Populates the given array from the starting index with the current
   * matrix values.
   * @param array defines the target array
   * @param offset defines the offset in the target array where to start storing
   * values
   * @returns the current matrix
   */
  const Matrix& copyToArray(Float32Array& array, unsigned int offset = 0) const;

  /**
   * @brief Sets the given matrix "result" with the multiplication result of the
   * current Matrix and the given one.
   * @param other defines the second operand
   * @param result defines the matrix where to store the multiplication
   * @returns the current matrix
   */
  Matrix& multiplyToRef(const Matrix& other, Matrix& result);

  /**
   * @brief Sets the Float32Array "result" from the given index "offset" with
   * the multiplication of the current matrix and the given one.
   * @param other defines the second operand
   * @param result defines the array where to store the multiplication
   * @param offset defines the offset in the target array where to start storing
   * values
   * @returns the current matrix
   */
  const Matrix& multiplyToArray(const Matrix& other, std::array<float, 16>& result,
                                unsigned int offset) const;

  /**
   * @brief Sets the Float32Array "result" from the given index "offset" with
   * the multiplication of the current matrix and the given one.
   * @param other defines the second operand
   * @param result defines the array where to store the multiplication
   * @param offset defines the offset in the target array where to start storing
   * values
   * @returns the current matrix
   */
  const Matrix& multiplyToArray(const Matrix& other, Float32Array& result,
                                unsigned int offset) const;

  /**
   * @brief Check equality between this matrix and a second one
   * @param value defines the second matrix to compare.
   * @returns true is the current matrix and the given one values are strictly
   * equal
   */
  [[nodiscard]] bool equals(const Matrix& value) const;

  /**
   * @brief Decomposes the current Matrix into a translation, rotation and
   * scaling components.
   * @param scale defines the scale vector3 given as a reference to update
   * @param rotation defines the rotation quaternion given as a reference to
   * update
   * @param translation defines the translation vector3 given as a reference to
   * update
   * @returns true if operation was successful
   */
  bool decompose(std::optional<Vector3>& scale, std::optional<Quaternion>& rotation,
                 std::optional<Vector3>& translation) const;

  /**
   * @brief Gets specific row of the matrix.
   * @param index defines the number of the row to get
   * @returns the index-th row of the current matrix as a new Vector4
   */
  [[nodiscard]] std::optional<Vector4> getRow(unsigned int index) const;

  /**
   * @brief Sets the index-th row of the current matrix to the vector4 values.
   * @param index defines the number of the row to set
   * @param row defines the target vector4
   * @returns the updated current matrix
   */
  Matrix& setRow(unsigned int index, const Vector4& row);

  /**
   * @brief Compute the transpose of the matrix.
   * @returns the new transposed matrix
   */
  [[nodiscard]] Matrix transpose() const;

  /**
   * @brief Compute the transpose of the matrix and store it in a given matrix.
   * @param result defines the target matrix
   * @returns the current matrix
   */
  Matrix& transposeToRef(Matrix& result);

  /**
   * @brief Sets the index-th row of the current matrix with the given 4 x float
   * values.
   * @param index defines the row index
   * @param x defines the x component to set
   * @param y defines the y component to set
   * @param z defines the z component to set
   * @param w defines the w component to set
   * @returns the updated current matrix
   */
  Matrix& setRowFromFloats(unsigned int index, float x, float y, float z, float w);

  /**
   * @brief Compute a new matrix set with the current matrix values multiplied
   * by scale (float).
   * @param scale defines the scale factor
   * @returns a new matrix
   */
  Matrix scale(float iscale);

  /**
   * @brief Scale the current matrix values by a factor to a given result
   * matrix.
   * @param scale defines the scale factor
   * @param result defines the matrix to store the result
   * @returns the current matrix
   */
  const Matrix& scaleToRef(float iscale, Matrix& result) const;

  /**
   * @brief Scale the current matrix values by a factor and add the result to a
   * given matrix.
   * @param scale defines the scale factor
   * @param result defines the Matrix to store the result
   * @returns the current matrix
   */
  const Matrix& scaleAndAddToRef(float iscale, Matrix& result) const;

  /**
   * @brief Writes to the given matrix a normal matrix, computed from this one
   * (using values from identity matrix for fourth row and column).
   * @param ref matrix to store the result
   */
  void toNormalMatrix(Matrix& ref);

  /**
   * @brief Gets only rotation part of the current matrix.
   * @returns a new matrix sets to the extracted rotation matrix from the
   * current one
   */
  [[nodiscard]] Matrix getRotationMatrix() const;

  /**
   * @brief Extracts the rotation matrix from the current one and sets it as the
   * given "result".
   * @param result defines the target matrix to store data to
   * @returns the current matrix
   */
  const Matrix& getRotationMatrixToRef(Matrix& result) const;

  /**
   * @brief Toggles model matrix from being right handed to left handed in place
   * and vice versa.
   */
  void toggleModelMatrixHandInPlace();

  /**
   * @brief Toggles projection matrix from being right handed to left handed in
   * place and vice versa.
   */
  void toggleProjectionMatrixHandInPlace();

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
   * @brief Creates a matrix from an array.
   * @param array defines the source array
   * @param offset defines an offset in the source array
   * @returns a new Matrix set from the starting index of the given array
   */
  static Matrix FromArray(const Float32Array& array, unsigned int offset = 0);

  /**
   * @brief Copy the content of an array into a given matrix.
   * @param array defines the source array
   * @param offset defines an offset in the source array
   * @param result defines the target matrix
   */
  static void FromArrayToRef(const Float32Array& array, unsigned int offset, Matrix& result);

  /**
   * @brief Stores an array into a matrix after having multiplied each component
   * by a given factor.
   * @param array defines the source array
   * @param offset defines the offset in the source array
   * @param scale defines the scaling factor
   * @param result defines the target matrix
   */
  static void FromFloat32ArrayToRefScaled(const Float32Array& array, unsigned int offset,
                                          float scale, Matrix& result);

  /**
   * @brief Gets an identity matrix that must not be updated.
   */
  static Matrix IdentityReadOnly();

  /**
   * @brief Stores a list of values (16) inside a given matrix.
   * @param initialM11 defines 1st value of 1st row
   * @param initialM12 defines 2nd value of 1st row
   * @param initialM13 defines 3rd value of 1st row
   * @param initialM14 defines 4th value of 1st row
   * @param initialM21 defines 1st value of 2nd row
   * @param initialM22 defines 2nd value of 2nd row
   * @param initialM23 defines 3rd value of 2nd row
   * @param initialM24 defines 4th value of 2nd row
   * @param initialM31 defines 1st value of 3rd row
   * @param initialM32 defines 2nd value of 3rd row
   * @param initialM33 defines 3rd value of 3rd row
   * @param initialM34 defines 4th value of 3rd row
   * @param initialM41 defines 1st value of 4th row
   * @param initialM42 defines 2nd value of 4th row
   * @param initialM43 defines 3rd value of 4th row
   * @param initialM44 defines 4th value of 4th row
   * @param result defines the target matrix
   */
  static void FromValuesToRef(float initialM11, float initialM12, float initialM13,
                              float initialM14, float initialM21, float initialM22,
                              float initialM23, float initialM24, float initialM31,
                              float initialM32, float initialM33, float initialM34,
                              float initialM41, float initialM42, float initialM43,
                              float initialM44, Matrix& result);

  /**
   * @brief Creates new matrix from a list of values (16).
   * @param initialM11 defines 1st value of 1st row
   * @param initialM12 defines 2nd value of 1st row
   * @param initialM13 defines 3rd value of 1st row
   * @param initialM14 defines 4th value of 1st row
   * @param initialM21 defines 1st value of 2nd row
   * @param initialM22 defines 2nd value of 2nd row
   * @param initialM23 defines 3rd value of 2nd row
   * @param initialM24 defines 4th value of 2nd row
   * @param initialM31 defines 1st value of 3rd row
   * @param initialM32 defines 2nd value of 3rd row
   * @param initialM33 defines 3rd value of 3rd row
   * @param initialM34 defines 4th value of 3rd row
   * @param initialM41 defines 1st value of 4th row
   * @param initialM42 defines 2nd value of 4th row
   * @param initialM43 defines 3rd value of 4th row
   * @param initialM44 defines 4th value of 4th row
   * @returns the new matrix
   */
  static Matrix FromValues(float initialM11, float initialM12, float initialM13, float initialM14,
                           float initialM21, float initialM22, float initialM23, float initialM24,
                           float initialM31, float initialM32, float initialM33, float initialM34,
                           float initialM41, float initialM42, float initialM43, float initialM44);

  /**
   * @brief Creates a new matrix composed by merging scale (vector3), rotation
   * (quaternion) and translation (vector3).
   * @param scale defines the scale vector3
   * @param rotation defines the rotation quaternion
   * @param translation defines the translation vector3
   * @returns a new matrix
   */
  static Matrix Compose(const Vector3& scale, const Quaternion& rotation,
                        const Vector3& translation);

  /**
   * @brief Sets a matrix to a value composed by merging scale (vector3),
   * rotation (quaternion) and translation (vector3).
   * @param scale defines the scale vector3
   * @param rotation defines the rotation quaternion
   * @param translation defines the translation vector3
   * @param result defines the target matrix
   */
  static void ComposeToRef(const Vector3& scale, const Quaternion& rotation,
                           const Vector3& translation, Matrix& result);

  /**
   * @brief Creates a new identity matrix.
   * @returns a new identity matrix
   */
  static Matrix Identity();

  /**
   * @brief Creates a new identity matrix and stores the result in a given
   * matrix.
   * @param result defines the target matrix
   */
  static void IdentityToRef(Matrix& result);

  /**
   * @brief Creates a new zero matrix.
   * @returns a new zero matrix
   */
  static Matrix Zero();

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the X axis.
   * @param angle defines the angle (in radians) to use
   * @return the new matrix
   */
  static Matrix RotationX(float angle);

  /**
   * @brief Creates a new matrix as the invert of a given matrix.
   * @param source defines the source matrix
   * @returns the new matrix
   */
  static Matrix Invert(Matrix& source);

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the X axis
   * and stores it in a given matrix.
   * @param angle defines the angle (in radians) to use
   * @param result defines the target matrix
   */
  static void RotationXToRef(float angle, Matrix& result);

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the Y axis.
   * @param angle defines the angle (in radians) to use
   * @return the new matrix
   */
  static Matrix RotationY(float angle);

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the Y axis
   * and stores it in a given matrix.
   * @param angle defines the angle (in radians) to use
   * @param result defines the target matrix
   */
  static void RotationYToRef(float angle, Matrix& result);

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the Z axis.
   * @param angle defines the angle (in radians) to use
   * @return the new matrix
   */
  static Matrix RotationZ(float angle);

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the Z axis.
   * and stores it in a given matrix
   * @param angle defines the angle (in radians) to use
   * @param result defines the target matrix
   */
  static void RotationZToRef(float angle, Matrix& result);

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the given.
   * axis
   * @param axis defines the axis to use
   * @param angle defines the angle (in radians) to use
   * @return the new matrix
   */
  static Matrix RotationAxis(Vector3& axis, float angle);

  /**
   * @brief Creates a new rotation matrix for "angle" radians around the given
   * axis and stores it in a given matrix.
   * @param axis defines the axis to use
   * @param angle defines the angle (in radians) to use
   * @param result defines the target matrix
   */
  static void RotationAxisToRef(Vector3& axis, float angle, Matrix& result);

  /**
   * @brief Takes normalised vectors and returns a rotation matrix to align
   * "from" with "to". Taken from
   * http://www.iquilezles.org/www/articles/noacos/noacos.htm
   * @param from defines the vector to align
   * @param to defines the vector to align to
   * @param result defines the target matrix
   */
  static void RotationAlignToRef(const Vector3& from, const Vector3& to, Matrix& result);

  /**
   * @brief Creates a rotation matrix.
   * @param yaw defines the yaw angle in radians (Y axis)
   * @param pitch defines the pitch angle in radians (X axis)
   * @param roll defines the roll angle in radians (X axis)
   * @returns the new rotation matrix
   */
  static Matrix RotationYawPitchRoll(float yaw, float pitch, float roll);

  /**
   * @brief Creates a rotation matrix and stores it in a given matrix.
   * @param yaw defines the yaw angle in radians (Y axis)
   * @param pitch defines the pitch angle in radians (X axis)
   * @param roll defines the roll angle in radians (X axis)
   * @param result defines the target matrix
   */
  static void RotationYawPitchRollToRef(float yaw, float pitch, float roll, Matrix& result);

  /**
   * @brief Creates a scaling matrix.
   * @param x defines the scale factor on X axis
   * @param y defines the scale factor on Y axis
   * @param z defines the scale factor on Z axis
   * @returns the new matrix
   */
  static Matrix Scaling(float x, float y, float z);

  /**
   * @brief Creates a scaling matrix and stores it in a given matrix.
   * @param x defines the scale factor on X axis
   * @param y defines the scale factor on Y axis
   * @param z defines the scale factor on Z axis
   * @param result defines the target matrix
   */
  static void ScalingToRef(float x, float y, float z, Matrix& result);

  /**
   * @brief Creates a translation matrix.
   * @param x defines the translation on X axis
   * @param y defines the translation on Y axis
   * @param z defines the translationon Z axis
   * @returns the new matrix
   */
  static Matrix Translation(float x, float y, float z);

  /**
   * @brief Creates a translation matrix and stores it in a given matrix.
   * @param x defines the translation on X axis
   * @param y defines the translation on Y axis
   * @param z defines the translationon Z axis
   * @param result defines the target matrix
   */
  static void TranslationToRef(float x, float y, float z, Matrix& result);

  /**
   * @brief Returns a new Matrix whose values are the interpolated values for
   * "gradient" (float) between the ones of the matrices "startValue" and
   * "endValue".
   * @param startValue defines the start value
   * @param endValue defines the end value
   * @param gradient defines the gradient factor
   * @returns the new matrix
   */
  static Matrix Lerp(const Matrix& startValue, const Matrix& endValue, float gradient);

  /**
   * @brief Set the given matrix "result" as the interpolated values for
   * "gradient" (float) between the ones of the matrices "startValue" and
   * "endValue".
   * @param startValue defines the start value
   * @param endValue defines the end value
   * @param gradient defines the gradient factor
   * @param result defines the Matrix object where to store data
   */
  static void LerpToRef(const Matrix& startValue, const Matrix& endValue, float gradient,
                        Matrix& result);

  /**
   * @brief Builds a new matrix whose values are computed by:
   * decomposing the the "startValue" and "endValue" matrices into their
   * respective scale, rotation and translation matrices interpolating for
   * "gradient" (float) the values between each of these decomposed matrices
   * between the start and the end recomposing a new matrix from these 3
   * interpolated scale, rotation and translation matrices.
   * @param startValue defines the first matrix
   * @param endValue defines the second matrix
   * @param gradient defines the gradient between the two matrices
   * @returns the new matrix
   */
  static Matrix DecomposeLerp(Matrix& startValue, Matrix& endValue, float gradient);

  /**
   * @brief Update a matrix to values which are computed by:
   * * decomposing the the "startValue" and "endValue" matrices into their
   * respective scale, rotation and translation matrices
   * * interpolating for "gradient" (float) the values between each of these
   * decomposed matrices between the start and the end
   * * recomposing a new matrix from these 3 interpolated scale, rotation and
   * translation matrices.
   * @param startValue defines the first matrix
   * @param endValue defines the second matrix
   * @param gradient defines the gradient between the two matrices
   * @param result defines the target matrix
   */
  static void DecomposeLerpToRef(Matrix& startValue, Matrix& endValue, float gradient,
                                 Matrix& result);

  /**
   * @brief Gets a new rotation matrix used to rotate an entity so as it looks
   * at the target vector3, from the eye vector3 position, the up vector3 being
   * oriented like "up". This function works in left handed mode
   * @param eye defines the final position of the entity
   * @param target defines where the entity should look at
   * @param up defines the up vector for the entity
   * @returns the new matrix
   */
  static Matrix LookAtLH(const Vector3& eye, Vector3& target, const Vector3& up);

  /**
   * @brief Sets the given "result" Matrix to a rotation matrix used to rotate
   * an entity so that it looks at the target vector3, from the eye vector3
   * position, the up vector3 being oriented like "up". This function works in
   * left handed mode
   * @param eye defines the final position of the entity
   * @param target defines where the entity should look at
   * @param up defines the up vector for the entity
   * @param result defines the target matrix
   */
  static void LookAtLHToRef(const Vector3& eye, const Vector3& target, const Vector3& up,
                            Matrix& result);

  /**
   * @brief Gets a new rotation matrix used to rotate an entity so as it looks
   * at the target vector3, from the eye vector3 position, the up vector3 being
   * oriented like "up". This function works in right handed mode
   * @param eye defines the final position of the entity
   * @param target defines where the entity should look at
   * @param up defines the up vector for the entity
   * @returns the new matrix
   */
  static Matrix LookAtRH(const Vector3& eye, Vector3& target, const Vector3& up);

  /**
   * @brief Sets the given "result" Matrix to a rotation matrix used to rotate
   * an entity so that it looks at the target vector3, from the eye vector3
   * position, the up vector3 being oriented like "up". This function works in
   * right handed mode
   * @param eye defines the final position of the entity
   * @param target defines where the entity should look at
   * @param up defines the up vector for the entity
   * @param result defines the target matrix
   */
  static void LookAtRHToRef(const Vector3& eye, const Vector3& target, const Vector3& up,
                            Matrix& result);

  /**
   * @brief Create a left-handed orthographic projection matrix.
   * @param width defines the viewport width
   * @param height defines the viewport height
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @returns a new matrix as a left-handed orthographic projection matrix
   */
  static Matrix OrthoLH(float width, float height, float znear, float zfar);

  /**
   * @brief Store a left-handed orthographic projection to a given matrix.
   * @param width defines the viewport width
   * @param height defines the viewport height
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @param result defines the target matrix
   */
  static void OrthoLHToRef(float width, float height, float znear, float zfar, Matrix& result);

  /**
   * @brief Create a left-handed orthographic projection matrix.
   * @param left defines the viewport left coordinate
   * @param right defines the viewport right coordinate
   * @param bottom defines the viewport bottom coordinate
   * @param top defines the viewport top coordinate
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @returns a new matrix as a left-handed orthographic projection matrix
   */
  static Matrix OrthoOffCenterLH(float left, float right, float bottom, float top, float znear,
                                 float zfar);

  /**
   * @brief Stores a left-handed orthographic projection into a given matrix.
   * @param left defines the viewport left coordinate
   * @param right defines the viewport right coordinate
   * @param bottom defines the viewport bottom coordinate
   * @param top defines the viewport top coordinate
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @param result defines the target matrix
   */
  static void OrthoOffCenterLHToRef(float left, float right, float bottom, float top, float znear,
                                    float zfar, Matrix& result);

  /**
   * @brief Creates a right-handed orthographic projection matrix.
   * @param left defines the viewport left coordinate
   * @param right defines the viewport right coordinate
   * @param bottom defines the viewport bottom coordinate
   * @param top defines the viewport top coordinate
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @returns a new matrix as a right-handed orthographic projection matrix
   */
  static Matrix OrthoOffCenterRH(float left, float right, float bottom, float top, float znear,
                                 float zfar);

  /**
   * @brief Stores a right-handed orthographic projection into a given matrix.
   * @param left defines the viewport left coordinate
   * @param right defines the viewport right coordinate
   * @param bottom defines the viewport bottom coordinate
   * @param top defines the viewport top coordinate
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @param result defines the target matrix
   */
  static void OrthoOffCenterRHToRef(float left, float right, float bottom, float top, float znear,
                                    float zfar, Matrix& result);

  /**
   * @brief Creates a left-handed perspective projection matrix.
   * @param width defines the viewport width
   * @param height defines the viewport height
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @returns a new matrix as a left-handed perspective projection matrix
   */
  static Matrix PerspectiveLH(float width, float height, float znear, float zfar);

  /**
   * @brief Creates a left-handed perspective projection matrix.
   * @param fov defines the horizontal field of view
   * @param aspect defines the aspect ratio
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @returns a new matrix as a left-handed perspective projection matrix
   */
  static Matrix PerspectiveFovLH(float fov, float aspect, float znear, float zfar);

  /**
   * @brief Stores a left-handed perspective projection into a given matrix.
   * @param fov defines the horizontal field of view
   * @param aspect defines the aspect ratio
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @param result defines the target matrix
   * @param isVerticalFovFixed defines it the fov is vertically fixed (default)
   * or horizontally
   */
  static void PerspectiveFovLHToRef(float fov, float aspect, float znear, float zfar,
                                    Matrix& result, bool isVerticalFovFixed = true);

  /**
   * @brief Creates a right-handed perspective projection matrix.
   * @param fov defines the horizontal field of view
   * @param aspect defines the aspect ratio
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @returns a new matrix as a right-handed perspective projection matrix
   */
  static Matrix PerspectiveFovRH(float fov, float aspect, float znear, float zfar);

  /**
   * @brief Stores a right-handed perspective projection into a given matrix.
   * @param fov defines the horizontal field of view
   * @param aspect defines the aspect ratio
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @param result defines the target matrix
   * @param isVerticalFovFixed defines it the fov is vertically fixed (default)
   * or horizontally
   */
  static void PerspectiveFovRHToRef(float fov, float aspect, float znear, float zfar,
                                    Matrix& result, bool isVerticalFovFixed = true);

  /**
   * @brief Stores a perspective projection for WebVR info a given matrix.
   * @param fov defines the field of view
   * @param znear defines the near clip plane
   * @param zfar defines the far clip plane
   * @param result defines the target matrix
   * @param rightHanded defines if the matrix must be in right-handed mode
   * (false by default)
   */
  static void PerspectiveFovWebVRToRef(const VRFov& fov, float znear, float zfar, Matrix& result,
                                       bool rightHanded = true);

  /**
   * @brief Computes a complete transformation matrix.
   * @param viewport defines the viewport to use
   * @param world defines the world matrix
   * @param view defines the view matrix
   * @param projection defines the projection matrix
   * @param zmin defines the near clip plane
   * @param zmax defines the far clip plane
   * @returns the transformation matrix
   */
  static Matrix GetFinalMatrix(const Viewport& viewport, Matrix& world, Matrix& view,
                               Matrix& projection, float zmin, float zmax);

  /**
   * @brief Extracts a 2x2 matrix from a given matrix and store the result in a
   * Float32Array.
   * @param matrix defines the matrix to use
   * @returns a new Float32Array array with 4 elements : the 2x2 matrix
   * extracted from the given matrix
   */
  static Float32Array GetAsMatrix2x2(const Matrix& matrix);

  /**
   * @brief Extracts a 3x3 matrix from a given matrix and store the result in a
   * Float32Array.
   * @param matrix defines the matrix to use
   * @returns a new Float32Array array with 9 elements : the 3x3 matrix
   * extracted from the given matrix
   */
  static Float32Array GetAsMatrix3x3(const Matrix& matrix);

  /**
   * @brief Compute the transpose of a given matrix.
   * @param matrix defines the matrix to transpose
   * @returns the new matrix
   */
  static Matrix Transpose(const Matrix& matrix);

  /**
   * @brief Compute the transpose of a matrix and store it in a target matrix.
   * @param matrix defines the matrix to transpose
   * @param result defines the target matrix
   */
  static void TransposeToRef(const Matrix& matrix, Matrix& result);

  /**
   * @brief Computes a reflection matrix from a plane.
   * @param plane defines the reflection plane
   * @returns a new matrix
   */
  static Matrix Reflection(Plane& plane);

  /**
   * @brief Computes a reflection matrix from a plane.
   * @param plane defines the reflection plane
   * @param result defines the target matrix
   */
  static void ReflectionToRef(Plane& plane, Matrix& result);

  /**
   * @brief Sets the given matrix as a rotation matrix composed from the 3 left
   * handed axes.
   * @param xaxis defines the value of the 1st axis
   * @param yaxis defines the value of the 2nd axis
   * @param zaxis defines the value of the 3rd axis
   * @param result defines the target matrix
   */
  static void FromXYZAxesToRef(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis,
                               Matrix& result);

  /**
   * @brief Creates a rotation matrix from a quaternion and stores it in a
   * target matrix.
   * @param quat defines the quaternion to use
   * @param result defines the target matrix
   */
  static void FromQuaternionToRef(const Quaternion& quat, Matrix& result);

private:
  /** @hidden */
  void _updateIdentityStatus(bool isIdentity, bool isIdentityDirty = false,
                             bool isIdentity3x2 = false, bool isIdentity3x2Dirty = true);

public:
  /**
   * Gets the update flag of the matrix which is an unique number for the
   * matrix. It will be incremented every time the matrix data change. You can
   * use it to speed the comparison between two versions of the same matrix.
   */
  int updateFlag;

#if BABYLONCPP_OPTION_ENABLE_SIMD == true
  SIMD::SIMDMatrix simdMatrix;
#endif

private:
  static int _updateFlagSeed;
  static Matrix _identityReadOnly;
  bool _isIdentity;
  bool _isIdentityDirty;
  bool _isIdentity3x2;
  bool _isIdentity3x2Dirty;

  std::array<float, 16> _m;

}; // end of class Matrix

std::ostream& operator<<(std::ostream& os, const Matrix& matrix);

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATRIX_H
