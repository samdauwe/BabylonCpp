#ifndef BABYLON_MATH_MATH_TMP_H
#define BABYLON_MATH_MATH_TMP_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Quaternion;
class Matrix;
class Vector3;

/**
 * @brief Same as Tmp but not exported to keep it only for math functions to
 * avoid conflicts.
 */
struct BABYLON_SHARED_EXPORT MathTmp {
  static std::array<Vector3, 6> Vector3Array;
  static std::array<Matrix, 2> MatrixArray;
  static std::array<Quaternion, 3> QuaternionArray;
}; // end of class MathTmp

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATH_TMP_H
