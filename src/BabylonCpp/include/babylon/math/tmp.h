#ifndef BABYLON_MATH_TMP_H
#define BABYLON_MATH_TMP_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Temporary pre-allocated objects for engine internal use.
 */
class BABYLON_SHARED_EXPORT Tmp {

public:
  static std::array<Color3, 3> Color3Array;
  static std::array<Vector2, 3> Vector2Array;
  static std::array<Vector3, 9> Vector3Array;
  static std::array<Vector4, 3> Vector4Array;
  static std::array<Quaternion, 2> QuaternionArray;
  static std::array<Matrix, 6> MatrixArray;

}; // end of class Tmp

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_TMP_H
