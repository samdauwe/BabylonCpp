#ifndef BABYLON_MATH_SIMD_MATRIX_H
#define BABYLON_MATH_SIMD_MATRIX_H
#ifdef OPTION_ENABLE_SIMD
#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Matrix;
class Vector3;

namespace SIMD {

struct BABYLON_SHARED_EXPORT SIMDMatrix {

  SIMDMatrix();
  SIMDMatrix(const SIMDMatrix& otherMatrix);
  SIMDMatrix(const Matrix& otherMatrix);
  SIMDMatrix(SIMDMatrix&& otherMatrix);
  SIMDMatrix(Matrix&& otherMatrix);
  SIMDMatrix& operator=(const SIMDMatrix& otherMatrix);
  SIMDMatrix& operator=(const Matrix& otherMatrix);
  SIMDMatrix& operator=(SIMDMatrix&& otherMatrix);
  SIMDMatrix& operator=(Matrix&& otherMatrix);
  ~SIMDMatrix();

  SIMDMatrix& multiplyToArraySIMD(const Matrix& other,
                                  std::array<float, 16>& result,
                                  unsigned int offset);
  SIMDMatrix& invertToRefSIMD(Matrix& other);
  static void LookAtLHToRefSIMD(const Vector3& eyeRef, const Vector3& targetRef,
                                const Vector3& upRef, Matrix& result);

  std::array<float, 16> m;

}; // end of struct SIMDMatrix

} // end of namespace SIMD
} // end of namespace BABYLON
#endif // #ifdef OPTION_ENABLE_SIMD
#endif // end of BABYLON_MATH_SIMD_MATRIX_H
