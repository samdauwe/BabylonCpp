#ifndef BABYLON_CULLING_BOUNDING_BOX_H
#define BABYLON_CULLING_BOUNDING_BOX_H

#include <babylon/babylon_global.h>
#include <babylon/culling/icullable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoundingBox : public ICullable {

public:
  BoundingBox(const Vector3& minimum, const Vector3& maximum);
  BoundingBox(const BoundingBox& other);
  virtual ~BoundingBox();

  /** Methods **/
  Matrix& getWorldMatrix();
  BoundingBox& setWorldMatrix(const Matrix& matrix);
  void _update(const Matrix& world);
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes) override;
  bool isCompletelyInFrustum(
    const std::array<Plane, 6>& frustumPlanes) const override;
  bool intersectsPoint(const Vector3& point);
  bool intersectsSphere(const BoundingSphere& sphere);
  bool intersectsMinMax(const Vector3& min, const Vector3& max) const;

  /** Statics **/
  static bool Intersects(const BoundingBox& box0, const BoundingBox& box1);
  static bool IntersectsSphere(const Vector3& minPoint, const Vector3& maxPoint,
                               const Vector3& sphereCenter, float sphereRadius);
  static bool IsCompletelyInFrustum(const std::vector<Vector3>& boundingVectors,
                                    const std::array<Plane, 6>& frustumPlanes);
  static bool IsInFrustum(const std::vector<Vector3>& boundingVectors,
                          const std::array<Plane, 6>& frustumPlanes);

public:
  std::vector<Vector3> vectors;
  Vector3 center;
  Vector3 extendSize;
  std::vector<Vector3> directions;
  std::vector<Vector3> vectorsWorld;
  Vector3 minimumWorld;
  Vector3 maximumWorld;
  Vector3 minimum;
  Vector3 maximum;

private:
  Matrix _worldMatrix;

}; // end of class BoundingBox

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_BOX_H
