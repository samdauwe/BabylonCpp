#ifndef BABYLON_CULLING_BOUNDING_SPHERE_H
#define BABYLON_CULLING_BOUNDING_SPHERE_H

#include <babylon/babylon_global.h>

#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoundingSphere {

public:
  BoundingSphere(const Vector3& minimum, const Vector3& maximum);
  BoundingSphere(const BoundingSphere& other);
  ~BoundingSphere();

  /** Methods **/
  void _update(const Matrix& world);
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) const;
  bool intersectsPoint(const Vector3& point);

  /** Statics **/
  static bool Intersects(const BoundingSphere& sphere0,
                         const BoundingSphere& sphere1);

public:
  Vector3 center;
  float radius;
  Vector3 centerWorld;
  float radiusWorld;

private:
  Vector3 _tempRadiusVector;

}; // end of class BoundingSphere

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_SPHERE_H
