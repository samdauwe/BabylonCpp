#ifndef BABYLON_MATH_MATH_TMP_H
#define BABYLON_MATH_MATH_TMP_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Same as Tmp but not exported to keep it only for math functions to
 * avoid conflicts.
 */
struct BABYLON_SHARED_EXPORT MathTmp {
  static array_t<Vector3, 6> Vector3Array;
  static array_t<Matrix, 2> MatrixArray;
  static array_t<Quaternion, 3> QuaternionArray;
}; // end of class MathTmp

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_MATH_TMP_H
