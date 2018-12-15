#ifndef BABYLON_CULLING_BOUNDING_SPHERE_H
#define BABYLON_CULLING_BOUNDING_SPHERE_H

#include <babylon/babylon_api.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Class used to store bounding sphere information.
 */
class BABYLON_SHARED_EXPORT BoundingSphere {

public:
  /**
   * @brief Creates a new bounding sphere
   * @param min defines the minimum vector (in local space)
   * @param max defines the maximum vector (in local space)
   */
  BoundingSphere(const Vector3& min, const Vector3& max);
  BoundingSphere(const BoundingSphere& other);
  BoundingSphere(BoundingSphere&& other);
  BoundingSphere& operator=(const BoundingSphere& other);
  BoundingSphere& operator=(BoundingSphere&& other);
  ~BoundingSphere();

  /**
   * @brief Recreates the entire bounding sphere from scratch.
   * @param min defines the new minimum vector (in local space)
   * @param max defines the new maximum vector (in local space)
   */
  void reConstruct(const Vector3& min, const Vector3& max);

  /**
   * @brief Scale the current bounding sphere by applying a scale factor.
   * @param factor defines the scale factor to apply
   * @returns the current bounding box
   */
  BoundingSphere& scale(float factor);

  /** Methods **/

  /**
   * @brief Hidden
   */
  void _update(const Matrix& world);

  /**
   * @brief Tests if the bounding sphere is intersecting the frustum planes.
   * @param frustumPlanes defines the frustum planes to test
   * @returns true if there is an intersection
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) const;

  /**
   * @brief Tests if a point is inside the bounding sphere.
   * @param point defines the point to test
   * @returns true if the point is inside the bounding sphere
   */
  bool intersectsPoint(const Vector3& point);

  /** Statics **/
  /**
   * @brief Checks if two sphere intersect.
   * @param sphere0 sphere 0
   * @param sphere1 sphere 1
   * @returns true if the speres intersect
   */
  static bool Intersects(const BoundingSphere& sphere0,
                         const BoundingSphere& sphere1);

public:
  /**
   * Gets the center of the bounding sphere in local space
   */
  Vector3 center;

  /**
   * Radius of the bounding sphere in local space
   */
  float radius;

  /**
   * Gets the center of the bounding sphere in world space
   */
  Vector3 centerWorld;

  /**
   * Radius of the bounding sphere in world space
   */
  float radiusWorld;

  /**
   * Gets the minimum vector in local space
   */
  Vector3 minimum;

  /**
   * Gets the maximum vector in local space
   */
  Vector3 maximum;

private:
  // This matrix is used as a value to reset the bounding box.
  Matrix _identityMatrix;
  Vector3 _tempRadiusVector;

}; // end of class BoundingSphere

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_SPHERE_H
