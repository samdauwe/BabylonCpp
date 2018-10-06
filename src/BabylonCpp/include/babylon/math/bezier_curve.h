#ifndef BABYLON_MATH_BEZIERCURVE_H
#define BABYLON_MATH_BEZIERCURVE_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to represent a Bezier curve.
 */
struct BABYLON_SHARED_EXPORT BezierCurve {

  /**
   * @brief Returns the cubic Bezier interpolated value (float) at "t" (float)
   * from the given x1, y1, x2, y2 floats.
   * @param t defines the time
   * @param x1 defines the left coordinate on X axis
   * @param y1 defines the left coordinate on Y axis
   * @param x2 defines the right coordinate on X axis
   * @param y2 defines the right coordinate on Y axis
   * @returns the interpolated value
   */
  static float Interpolate(float t, float x1, float y1, float x2, float y2);

}; // end of class BezierCurve

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_BEZIERCURVE_H
