#ifndef BABYLON_CULLING_BOUNDING_BOX_H
#define BABYLON_CULLING_BOUNDING_BOX_H

#include <babylon/babylon_api.h>
#include <babylon/culling/icullable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BoundingSphere;

/**
 * @brief Class used to store bounding box information.
 */
class BABYLON_SHARED_EXPORT BoundingBox : public ICullable {

public:
  /**
   * @brief Creates a new bounding box
   * @param min defines the minimum vector (in local space)
   * @param max defines the maximum vector (in local space)
   * @param worldMatrix defines the new world matrix
   */
  BoundingBox(const Vector3& min, const Vector3& max,
              const std::optional<Matrix>& worldMatrix = std::nullopt);
  BoundingBox(const BoundingBox& other);
  BoundingBox(BoundingBox&& other);
  BoundingBox& operator=(const BoundingBox& other);
  BoundingBox& operator=(BoundingBox&& other);
  ~BoundingBox() override; // = default

  // Methods

  /**
   * @brief Recreates the entire bounding box from scratch as if we call the
   * constructor in place.
   * @param min defines the new minimum vector (in local space)
   * @param max defines the new maximum vector (in local space)
   * @param worldMatrix defines the new world matrix
   */
  void reConstruct(Vector3 min, Vector3 max,
                   const std::optional<Matrix>& worldMatrix = std::nullopt);

  /**
   * @brief Scale the current bounding box by applying a scale factor.
   * @param factor defines the scale factor to apply
   * @returns the current bounding box
   */
  BoundingBox& scale(float factor);

  /**
   * @brief Gets the world matrix of the bounding box.
   * @returns a matrix
   */
  Matrix& getWorldMatrix();

  /**
   * @brief Hidden
   */
  void _update(const Matrix& world);

  /**
   * @brief Tests if the bounding box is intersecting the frustum planes.
   * @param frustumPlanes defines the frustum planes to test
   * @returns true if there is an intersection
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes, unsigned int strategy = 0) override;

  /**
   * @brief Tests if the bounding box is entirely inside the frustum planes.
   * @param frustumPlanes defines the frustum planes to test
   * @returns true if there is an inclusion
   */
  bool isCompletelyInFrustum(const std::array<Plane, 6>& frustumPlanes) override;

  /**
   * @brief Tests if a point is inside the bounding box.
   * @param point defines the point to test
   * @returns true if the point is inside the bounding box
   */
  bool intersectsPoint(const Vector3& point);

  /**
   * @brief Tests if the bounding box intersects with a bounding sphere.
   * @param sphere defines the sphere to test
   * @returns true if there is an intersection
   */
  bool intersectsSphere(const BoundingSphere& sphere);

  /**
   * @brief Tests if the bounding box intersects with a box defined by a min and
   * max vectors.
   * @param min defines the min vector to use
   * @param max defines the max vector to use
   * @returns true if there is an intersection
   */
  bool intersectsMinMax(const Vector3& min, const Vector3& max) const;

  /** Statics **/
  /**
   * @brief Tests if two bounding boxes are intersections.
   * @param box0 defines the first box to test
   * @param box1 defines the second box to test
   * @returns true if there is an intersection
   */
  static bool Intersects(const BoundingBox& box0, const BoundingBox& box1);

  /**
   * @brief Tests if a bounding box defines by a min/max vectors intersects a
   * sphere.
   * @param minPoint defines the minimum vector of the bounding box
   * @param maxPoint defines the maximum vector of the bounding box
   * @param sphereCenter defines the sphere center
   * @param sphereRadius defines the sphere radius
   * @returns true if there is an intersection
   */
  static bool IntersectsSphere(const Vector3& minPoint, const Vector3& maxPoint,
                               const Vector3& sphereCenter, float sphereRadius);

  /**
   * @brief Tests if a bounding box defined with 8 vectors is entirely inside
   * frustum planes.
   * @param boundingVectors defines an array of 8 vectors representing a
   * bounding box
   * @param frustumPlanes defines the frustum planes to test
   * @return true if there is an inclusion
   */
  static bool IsCompletelyInFrustum(const std::array<Vector3, 8>& boundingVectors,
                                    const std::array<Plane, 6>& frustumPlanes);

  /**
   * @brief Tests if a bounding box defined with 8 vectors intersects frustum
   * planes.
   * @param boundingVectors defines an array of 8 vectors representing a
   * bounding box
   * @param frustumPlanes defines the frustum planes to test
   * @return true if there is an intersection
   */
  static bool IsInFrustum(const std::array<Vector3, 8>& boundingVectors,
                          const std::array<Plane, 6>& frustumPlanes);

public:
  /**
   * Gets the 8 vectors representing the bounding box in local space
   */
  std::array<Vector3, 8> vectors;

  /**
   * Gets the center of the bounding box in local space
   */
  Vector3 center;

  /**
   * Gets the center of the bounding box in world space
   */
  Vector3 centerWorld;

  /**
   * Gets the extend size in local space
   */
  Vector3 extendSize;

  /**
   * Gets the extend size in world space
   */
  Vector3 extendSizeWorld;

  /**
   * Gets the OBB (object bounding box) directions
   */
  std::array<Vector3, 3> directions;

  /**
   * Gets the 8 vectors representing the bounding box in world space
   */
  std::array<Vector3, 8> vectorsWorld;

  /**
   * Gets the minimum vector in world space
   */
  Vector3 minimumWorld;

  /**
   * Gets the maximum vector in world space
   */
  Vector3 maximumWorld;

  /**
   * Gets the minimum vector in local space
   */
  Vector3 minimum;

  /**
   * Gets the maximum vector in local space
   */
  Vector3 maximum;

  /**
   * Hidden
   */
  int _tag;

private:
  Matrix _worldMatrix;
  static std::array<Vector3, 3> TmpVector3;

}; // end of class BoundingBox

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_BOX_H
