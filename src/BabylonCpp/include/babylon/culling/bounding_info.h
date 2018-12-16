#ifndef BABYLON_CULLING_BOUNDING_INFO_H
#define BABYLON_CULLING_BOUNDING_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/culling/icullable.h>

namespace BABYLON {

class Collider;

/**
 * @brief Info for a bounding data of a mesh.
 */
class BABYLON_SHARED_EXPORT BoundingInfo : public ICullable {

public:
  /**
   * @brief Constructs bounding info.
   * @param minimum min vector of the bounding box/sphere
   * @param maximum max vector of the bounding box/sphere
   */
  BoundingInfo(const Vector3& minimum, const Vector3& maximum);
  BoundingInfo(const BoundingInfo& boundingInfo);
  BoundingInfo(BoundingInfo&& other);
  BoundingInfo& operator=(const BoundingInfo& other);
  BoundingInfo& operator=(BoundingInfo&& other);
  virtual ~BoundingInfo();

  /** Methods **/
  /**
   * @brief Updates the boudning sphere and box.
   * @param world world matrix to be used to update
   */
  void update(const Matrix& world);

  /**
   * @brief Recreate the bounding info to be centered around a specific point
   * given a specific extend.
   * @param center New center of the bounding info
   * @param extend New extend of the bounding info
   * @returns the current bounding info
   */
  BoundingInfo& centerOn(const Vector3& center, const Vector3& extend);

  /**
   * @brief Scale the current bounding info by applying a scale factor.
   * @param factor defines the scale factor to apply
   * @returns the current bounding info
   */
  BoundingInfo& scale(float factor);

  /**
   * @brief Returns `true` if the bounding info is within the frustum defined by
   * the passed array of planes.
   * @param frustumPlanes defines the frustum to test
   * @param strategy defines the strategy to use for the culling (default is
   * BABYLON.Scene.CULLINGSTRATEGY_STANDARD)
   * @returns true if the bounding info is in the frustum planes
   */
  bool isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                   unsigned int strategy
                   = 0 /*AbstractMesh::CULLINGSTRATEGY_STANDARD*/) override;

  /**
   * @brief Gets the world distance between the min and max points of the
   * bounding box.
   */
  float diagonalLength() const;

  /**
   * @brief Checks if a cullable object (mesh...) is in the camera frustum.
   * Unlike isInFrustum this cheks the full bounding box
   * @param frustumPlanes Camera near/planes
   * @returns true if the object is in frustum otherwise false
   */
  bool isCompletelyInFrustum(
    const std::array<Plane, 6>& frustumPlanes) const override;

  /**
   * @brief Hidden
   */
  bool _checkCollision(const Collider& collider) const;

  /**
   * @brief Checks if a point is inside the bounding box and bounding sphere or
   * the mesh.
   * @see https://doc.babylonjs.com/babylon101/intersect_collisions_-_mesh
   * @param point the point to check intersection with
   * @returns if the point intersects
   */
  bool intersectsPoint(const Vector3& point);

  /**
   * @brief Checks if another bounding info intersects the bounding box and
   * bounding sphere or the mesh.
   * @see https://doc.babylonjs.com/babylon101/intersect_collisions_-_mesh
   * @param boundingInfo the bounding info to check intersection with
   * @param precise if the intersection should be done using OBB
   * @returns if the bounding info intersects
   */
  bool intersects(const BoundingInfo& boundingInfo, bool precise);

protected:
  Vector3& get_minimum();
  Vector3& get_maximum();
  bool get_isLocked() const;
  void set_isLocked(bool value);

private:
  Extents computeBoxExtents(const Vector3& axis, const BoundingBox& box) const;
  bool extentsOverlap(float min0, float max0, float min1, float max1) const;
  bool axisOverlap(const Vector3& axis, const BoundingBox& box0,
                   const BoundingBox& box1) const;

public:
  /**
   * Bounding box for the mesh
   */
  BoundingBox boundingBox;

  /**
   * Bounding sphere for the mesh
   */
  BoundingSphere boundingSphere;

  /**
   * min vector of the bounding box/sphere
   */
  ReadOnlyProperty<BoundingInfo, Vector3> minimum;

  /**
   * max vector of the bounding box/sphere
   */
  ReadOnlyProperty<BoundingInfo, Vector3> maximum;

  /**
   * If the info is locked and won't be updated to avoid perf overhead
   */
  Property<BoundingInfo, bool> isLocked;

private:
  bool _isLocked;

}; // end of class BoundingInfo

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_BOUNDING_INFO_H
