#ifndef BABYLON_MATH_TMP_H
#define BABYLON_MATH_TMP_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Color3;
class Color4;
class Matrix;
class Quaternion;
class Vector2;
class Vector3;
class Vector4;

/**
 * @brief Temporary pre-allocated objects for engine internal use.
 * Hidden
 */
struct BABYLON_SHARED_EXPORT Tmp {
  static std::array<Color3, 3> Color3Array;
  static std::array<Color4, 3> Color4Array;
  // 3 temp Vector2 at once should be enough
  static std::array<Vector2, 3> Vector2Array;
  // 13 temp Vector3 at once should be enough
  static std::array<Vector3, 13> Vector3Array;
  // 3 temp Vector4 at once should be enough
  static std::array<Vector4, 3> Vector4Array;
  // 2 temp Quaternion at once should be enough
  static std::array<Quaternion, 2> QuaternionArray;
  // 8 temp Matrices at once should be enough
  static std::array<Matrix, 8> MatrixArray;
}; // end of struct Tmp

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_TMP_H
