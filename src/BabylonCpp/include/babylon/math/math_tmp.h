#ifndef BABYLON_MATH_MATH_TMP_H
#define BABYLON_MATH_MATH_TMP_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Same as Tmp but not exported to keep it onyl for math functions to
 * avoid conflicts.
 */
struct BABYLON_SHARED_EXPORT MathTmp {

  static std::array<Vector3, 1> Vector3Array;
  static std::array<Matrix, 2> MatrixArray;
  static std::array<Quaternion, 1> QuaternionArray;

}; // end of class MathTmp

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATH_TMP_H
