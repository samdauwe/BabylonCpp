#ifndef BABYLON_CULLING_BOUNDING_SPHERE_H
#define BABYLON_CULLING_BOUNDING_SPHERE_H

#include <babylon/babylon_global.h>

#include <babylon/math/vector3.h>

namespace BABYLON {

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
  void _update(const Matrix& world);
  bool isInFrustum(const array_t<Plane, 6>& frustumPlanes) const;
  bool intersectsPoint(const Vector3& point);

  /** Statics **/
  static bool Intersects(const BoundingSphere& sphere0,
                         const BoundingSphere& sphere1);

public:
  Vector3 center;
  float radius;
  Vector3 centerWorld;
  float radiusWorld;
  Vector3 minimum;
  Vector3 maximum;

private:
  Vector3 _tempRadiusVector;

}; // end of class BoundingSphere

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_SPHERE_H
