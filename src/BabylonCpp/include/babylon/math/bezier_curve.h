#ifndef BABYLON_MATH_BEZIERCURVE_H
#define BABYLON_MATH_BEZIERCURVE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a BezierCurve.
 */
struct BABYLON_SHARED_EXPORT BezierCurve {

  /**
   * @brief Returns the cubic Bezier interpolated value (float) at "t" (float)
   * from the given x1, y1, x2, y2 floats.
   */
  static float Interpolate(float t, float x1, float y1, float x2, float y2);

}; // end of class BezierCurve

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_BEZIERCURVE_H
