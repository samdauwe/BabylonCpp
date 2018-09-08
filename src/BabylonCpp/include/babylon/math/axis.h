#ifndef BABYLON_MATH_AXIS_H
#define BABYLON_MATH_AXIS_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Defines the 3 main axes.
 */
struct BABYLON_SHARED_EXPORT Axis {

  /** Statics **/

  /** X axis */
  static const Vector3 X()
  {
    return Vector3(1.f, 0.f, 0.f);
  }

  /** Y axis */
  static const Vector3 Y()
  {
    return Vector3(0.f, 1.f, 0.f);
  }

  /** Z axis */
  static const Vector3 Z()
  {
    return Vector3(0.f, 0.f, 1.f);
  }

}; // end of struct Axis

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_AXIS_H
