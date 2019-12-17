#ifndef BABYLON_MATHS_FRUSTUM_H
#define BABYLON_MATHS_FRUSTUM_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Matrix;
class Plane;

/**
 * @brief Represents a camera frustum.
 */
class BABYLON_SHARED_EXPORT Frustum {

public:
  /** Statics **/

  /**
   * @brief Gets the planes representing the frustum
   * @param transform matrix to be applied to the returned planes
   * @returns a new array of 6 Frustum planes computed by the given transformation matrix.
   */
  static std::array<Plane, 6> GetPlanes(const Matrix& transform);

  /**
   * @brief Gets the near frustum plane transformed by the transform matrix
   * @param transform transformation matrix to be applied to the resulting frustum plane
   * @param frustumPlane the resuling frustum plane
   */
  static void GetNearPlaneToRef(const Matrix& transform, Plane& frustumPlane);

  /**
   * @brief Gets the far frustum plane transformed by the transform matrix.
   * @param transform transformation matrix to be applied to the resulting frustum plane
   * @param frustumPlane the resuling frustum plane
   */
  static void GetFarPlaneToRef(const Matrix& transform, Plane& frustumPlane);

  /**
   * @brief Gets the left frustum plane transformed by the transform matrix.
   * @param transform transformation matrix to be applied to the resulting frustum plane
   * @param frustumPlane the resuling frustum plane
   */
  static void GetLeftPlaneToRef(const Matrix& transform, Plane& frustumPlane);

  /**
   * @brief Gets the right frustum plane transformed by the transform matrix.
   * @param transform transformation matrix to be applied to the resulting frustum plane
   * @param frustumPlane the resuling frustum plane
   */
  static void GetRightPlaneToRef(const Matrix& transform, Plane& frustumPlane);

  /**
   * @brief Gets the top frustum plane transformed by the transform matrix.
   * @param transform transformation matrix to be applied to the resulting frustum plane
   * @param frustumPlane the resuling frustum plane
   */
  static void GetTopPlaneToRef(const Matrix& transform, Plane& frustumPlane);

  /**
   * @brief Gets the bottom frustum plane transformed by the transform matrix.
   * @param transform transformation matrix to be applied to the resulting frustum plane
   * @param frustumPlane the resuling frustum plane
   */
  static void GetBottomPlaneToRef(const Matrix& transform, Plane& frustumPlane);

  /**
   * @brief Sets the given array "frustumPlanes" with the 6 Frustum planes computed by the given
   * transformation matrix.
   * @param transform transformation matrix to be applied to the resulting frustum planes
   * @param frustumPlanes the resuling frustum planes
   */
  static void GetPlanesToRef(const Matrix& transform, std::array<Plane, 6>& frustumPlanes);

}; // end of class Frustum

} // end of namespace BABYLON

#endif // end of BABYLON_MATHS_FRUSTUM_H
