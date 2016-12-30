#ifndef BABYLON_MATH_BEZIERCURVE_H
#define BABYLON_MATH_BEZIERCURVE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a BezierCurve.
 */
class BABYLON_SHARED_EXPORT BezierCurve {

public:
  static float Interpolate(float t, float x1, float y1, float x2, float y2);

}; // end of class BezierCurve

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_BEZIERCURVE_H
