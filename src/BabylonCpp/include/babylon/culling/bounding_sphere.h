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
   * @param worldMatrix defines the new world matrix
   */
  BoundingSphere(const Vector3& min, const Vector3& max,
                 const std::optional<Matrix>& worldMatrix = std::nullopt);
  BoundingSphere(const BoundingSphere& other);
  BoundingSphere(BoundingSphere&& other);
  BoundingSphere& operator=(const BoundingSphere& other);
  BoundingSphere& operator=(BoundingSphere&& other);
  ~BoundingSphere();

  /**
   * @brief Recreates the entire bounding sphere from scratch as if we call the
   * constructor in place.
   * @param min defines the new minimum vector (in local space)
   * @param max defines the new maximum vector (in local space)
   * @param worldMatrix defines the new world matrix
   */
  void reConstruct(const Vector3& min, Vector3 max,
                   const std::optional<Matrix>& worldMatrix = std::nullopt);

  /**
   * @brief Scale the current bounding sphere by applying a scale factor.
   * @param factor defines the scale factor to apply
   * @returns the current bounding box
   */
  BoundingSphere& scale(float factor);

  /**
   * @brief Gets the world matrix of the bounding box.
   * @returns a matrix
   */
  Matrix& getWorldMatrix();

  /** Methods **/

  /**
   * @brief Hidden
   */
  void _update(const Matrix& worldMatrix);

  /**
   * @brief Tests if the bounding sphere is intersecting the frustum planes.
   * @param frustumPlanes defines the frustum planes to test
   * @returns true if there is an intersection
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) const;

  /**
   * @brief Tests if the bounding sphere center is in between the frustum
   * planes. Used for optimistic fast inclusion.
   * @param frustumPlanes defines the frustum planes to test
   * @returns true if the sphere center is in between the frustum planes
   */
  bool isCenterInFrustum(const std::array<Plane, 6>& frustumPlanes) const;

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
  Matrix _worldMatrix;
  static std::array<Vector3, 3> TmpVector3;

}; // end of class BoundingSphere

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_SPHERE_H
