#ifndef OIMO_CONSTRAINT_CONTACT_CONTACT_MANIFOLD_H
#define OIMO_CONSTRAINT_CONTACT_CONTACT_MANIFOLD_H

#include <array>

#include <oimo/constraint/contact/manifold_point.h>

namespace OIMO {

class RigidBody;
class Shape;

/**
 * @brief A contact manifold between two shapes.
 */
class ContactManifold {

public:
  ContactManifold();
  ~ContactManifold();

  /**
   * @brief Reset the manifold.
   * @param   shape1
   * @param   shape2
   */
  void reset(Shape* shape1, Shape* shape2);

  /**
   * @brief Add a point into this manifold.
   * @param pos
   * @param norm
   * @param penetration
   * @param flip
   */
  void addPointVec(const Vec3& pos, const Vec3& norm, bool penetration,
                   bool flip);

  /**
   * @brief Add a point into this manifold.
   * @param   x
   * @param   y
   * @param   z
   * @param   normalX
   * @param   normalY
   * @param   normalZ
   * @param   penetration
   * @param   flip
   */
  void addPoint(float x, float y, float z, float normalX = 0.f,
                float normalY = 0.f, float normalZ = 0.f,
                float penetration = false, bool flip = false);

public:
  // The manifold points.
  std::array<ManifoldPoint, 4> points;
  // The number of manifold points.
  unsigned int numPoints;
  // The first rigid body.
  RigidBody* body1;
  // The second rigid body.
  RigidBody* body2;

}; // end of class ContactManifold

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_CONTACT_CONTACT_MANIFOLD_H
