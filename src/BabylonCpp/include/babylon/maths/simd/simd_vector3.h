#ifndef BABYLON_MATH_SIMD_VECTOR3_H
#define BABYLON_MATH_SIMD_VECTOR3_H

#include <babylon/babylon_api.h>

namespace BABYLON {

class Matrix;
class Vector3;

namespace SIMD {

struct BABYLON_SHARED_EXPORT SIMDVector3 {

  static void TransformCoordinatesToRefSIMD(const Vector3& vector,
                                            const Matrix& transformation,
                                            Vector3& result);

  static void TransformCoordinatesFromFloatsToRefSIMD(
    float x, float y, float z, const Matrix& transformation, Vector3& result);

}; // end of struct SIMDVector3

} // end of namespace SIMD
} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SIMD_VECTOR3_H
