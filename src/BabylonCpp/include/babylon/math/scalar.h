#ifndef BABYLON_MATH_SCALAR_H
#define BABYLON_MATH_SCALAR_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT Scalar {

  /**
   * @brief Creates a new scalar with values linearly interpolated of "amount"
   * between the start scalar and the end scalar.
   */
  static float Lerp(float start, float end, float amount);

  /**
   * @brief Returns a new scalar located for "amount" (float) on the Hermite
   * spline defined by the scalars "value1", "value3", "tangent1", "tangent2".
   */
  static float Hermite(float value1, float tangent1, float value2,
                       float tangent2, float amount);

}; // end of struct Scalar

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_SCALAR_H
