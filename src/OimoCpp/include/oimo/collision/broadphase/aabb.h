#ifndef OIMO_COLLISION_BROADPHASE_AABB_H
#define OIMO_COLLISION_BROADPHASE_AABB_H

#include <array>
#include <vector>

#include <oimo/oimo_api.h>
#include <oimo/shape/vertex.h>

namespace OIMO {

class Vec3;

/**
 * @brief An axis-aligned bounding box.
 */
class OIMO_SHARED_EXPORT AABB {

public:
  // AABB aproximation
  static const float AABB_PROX;

public:
  AABB(float minX = 0, float maxX = 0, float minY = 0, float maxY = 0,
       float minZ = 0, float maxZ = 0);
  AABB(const AABB& aabb);
  AABB(AABB&& aabb);
  ~AABB();
  AABB& operator=(const AABB& aabb);
  AABB& operator=(AABB&& aabb);
  AABB clone() const;
  AABB* cloneToNewObject() const;

  AABB& set(float minX, float maxX, float minY, float maxY, float minZ,
            float maxZ);
  bool intersectTest(const AABB& aabb) const;
  bool intersectTestTwo(const AABB& aabb) const;
  AABB& copy(const AABB& aabb, float margin = 0.f);
  AABB& fromArray(const std::array<float, 6>& array);

  /**
   * Set this AABB to the combined AABB of aabb1 and aabb2.
   */
  AABB& combine(const AABB& aabb1, const AABB& aabb2);

  /**
   * Set this AABB to the combined AABB of aabb1 and aabb2.
   */
  float surfaceArea() const;

  /**
   * Get whether the AABB intersects with the point or not.
   */
  bool intersectsWithPoint(float x, float y, float z) const;

  /**
   * Set the AABB from an array of vertices
   */
  void setFromPoints(const std::vector<Vertex>& arr);
  void setFromPoints(const std::vector<Vec3>& arr);
  void setFromPoints(const std::array<Vertex, 4>& arr);
  void setFromPoints(const std::array<Vec3, 4>& arr);
  void makeEmpty();
  void expandByPoint(const Vec3& pt);
  void expandByScalar(float s);

public:
  std::array<float, 6> elements;

}; // end of class AABB

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BASE_ANGULAR_CONSTRAINT_H
