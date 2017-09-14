#ifndef BABYLON_MATH_FRUSTUM_H
#define BABYLON_MATH_FRUSTUM_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Represents a Frustum.
 */
class BABYLON_SHARED_EXPORT Frustum {

public:
  /** Statics **/

  /**
   * @brief Returns a new array of 6 Frustum planes computed by the passed
   * transformation matrix.
   */
  static array_t<Plane, 6> GetPlanes(const Matrix& transform);

  /**
   * @brief Sets the passed array "frustumPlanes" with the 6 Frustum planes
   * computed by the passed transformation matrix.
   */
  static void GetPlanesToRef(const Matrix& transform,
                             array_t<Plane, 6>& frustumPlanes);

}; // end of class Frustum

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_FRUSTUM_H
