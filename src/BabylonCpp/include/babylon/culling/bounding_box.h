#ifndef BABYLON_CULLING_BOUNDING_BOX_H
#define BABYLON_CULLING_BOUNDING_BOX_H

#include <babylon/babylon_global.h>
#include <babylon/culling/icullable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoundingBox : public ICullable {

public:
  /**
   * @brief Creates a new bounding box.
   * @param min defines the minimum vector (in local space)
   * @param max defines the maximum vector (in local space)
   */
  BoundingBox(const Vector3& min, const Vector3& max);
  BoundingBox(const BoundingBox& other);
  BoundingBox(BoundingBox&& other);
  BoundingBox& operator=(const BoundingBox& other);
  BoundingBox& operator=(BoundingBox&& other);
  virtual ~BoundingBox();

  /** Methods **/

  /**
   * @brief Recreates the entire bounding box from scratch.
   * @param min defines the new minimum vector (in local space)
   * @param max defines the new maximum vector (in local space)
   */
  void reConstruct(const Vector3& min, const Vector3& max);

  Matrix& getWorldMatrix();
  BoundingBox& setWorldMatrix(const Matrix& matrix);
  void _update(const Matrix& world);
  bool isInFrustum(const array_t<Plane, 6>& frustumPlanes) override;
  bool
  isCompletelyInFrustum(const array_t<Plane, 6>& frustumPlanes) const override;
  bool intersectsPoint(const Vector3& point);
  bool intersectsSphere(const BoundingSphere& sphere);
  bool intersectsMinMax(const Vector3& min, const Vector3& max) const;

  /** Statics **/
  static bool Intersects(const BoundingBox& box0, const BoundingBox& box1);
  static bool IntersectsSphere(const Vector3& minPoint, const Vector3& maxPoint,
                               const Vector3& sphereCenter, float sphereRadius);
  static bool IsCompletelyInFrustum(const vector_t<Vector3>& boundingVectors,
                                    const array_t<Plane, 6>& frustumPlanes);
  static bool IsInFrustum(const vector_t<Vector3>& boundingVectors,
                          const array_t<Plane, 6>& frustumPlanes);

public:
  vector_t<Vector3> vectors;
  Vector3 center;
  Vector3 centerWorld;
  Vector3 extendSize;
  Vector3 extendSizeWorld;
  vector_t<Vector3> directions;
  vector_t<Vector3> vectorsWorld;
  Vector3 minimumWorld;
  Vector3 maximumWorld;

  Vector3 minimum;
  Vector3 maximum;

private:
  Matrix _worldMatrix;

}; // end of class BoundingBox

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_BOX_H
