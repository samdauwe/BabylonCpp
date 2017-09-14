#ifndef BABYLON_MATH_TMP_H
#define BABYLON_MATH_TMP_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Temporary pre-allocated objects for engine internal use.
 */
struct BABYLON_SHARED_EXPORT Tmp {
  static array_t<Color3, 3> Color3Array;
  static array_t<Vector2, 3> Vector2Array;
  static array_t<Vector3, 9> Vector3Array;
  static array_t<Vector4, 3> Vector4Array;
  static array_t<Quaternion, 2> QuaternionArray;
  static array_t<Matrix, 6> MatrixArray;
}; // end of struct Tmp

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_TMP_H
