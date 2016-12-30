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
  static std::array<Plane, 6> GetPlanes(const Matrix& transform);
  static void GetPlanesToRef(const Matrix& transform,
                             std::array<Plane, 6>& frustumPlanes);

}; // end of class Frustum

} // end of namespace BABYLON

#endif // end of BABYLON_MATH_FRUSTUM_H
