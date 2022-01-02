#ifndef BABYLON_COLLISIONS_COLLIDER_H
#define BABYLON_COLLISIONS_COLLIDER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/structs.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

FWD_CLASS_SPTR(AbstractMesh)

/**
 * @brief Implementation of the "Improved Collision detection and Response" algorithm proposed by
 * Kasper Fauerby.
 * @see https://www.peroxide.dk/papers/collision/collision.pdf
 */
class BABYLON_SHARED_EXPORT Collider {

public:
  Collider();
  ~Collider(); // = default

  /** Statics **/
  static bool IntersectBoxAASphere(const Vector3& boxMin, const Vector3& boxMax,
                                   const Vector3& sphereCenter, float sphereRadius);
  static LowestRoot GetLowestRoot(float a, float b, float c, float maxR);

  /**
   * @brief Gets the plane normal used to compute the sliding response (in local
   * space)
   */
  Vector3& slidePlaneNormal();

  /** Methods **/
  /** Hidden */
  void _initialize(Vector3& source, Vector3& dir, float e);
  /** Hidden */
  bool _checkPointInTriangle(const Vector3& point, const Vector3& pa, const Vector3& pb,
                             const Vector3& pc, const Vector3& n);
  /** Hidden */
  [[nodiscard]] bool _canDoCollision(const Vector3& sphereCenter, float sphereRadius,
                                     const Vector3& vecMin, const Vector3& vecMax) const;
  /** Hidden */
  void _testTriangle(size_t faceIndex, std::vector<Plane>& trianglePlaneArray, const Vector3& p1,
                     const Vector3& p2, const Vector3& p3, bool hasMaterial,
                     const AbstractMeshPtr& hostMesh);
  /** Hidden */
  void _collide(std::vector<Plane>& trianglePlaneArray, const std::vector<Vector3>& pts,
                const IndicesArray& indices, size_t indexStart, size_t indexEnd, unsigned int decal,
                bool hasMaterial, const AbstractMeshPtr& hostMesh);
  /** Hidden */
  void _getResponse(Vector3& pos, Vector3& vel);

protected:
  [[nodiscard]] int get_collisionMask() const;
  void set_collisionMask(int mask);

public:
  /**
   * Define if a collision was found
   */
  bool collisionFound;

  /**
   * Define last intersection point in local space
   */
  Vector3 intersectionPoint;
  bool intersectionPointSet;

  /**
   * Define last collided mesh
   */
  AbstractMeshPtr collidedMesh;

  unsigned int collidedMeshId;
  /** Hidden */
  Vector3 _radius;
  /** Hidden */
  unsigned int _retry;
  /** Hidden */
  float _velocityWorldLength;
  /** Hidden */
  Vector3 _basePointWorld;
  /** Hidden */
  Vector3 _initialVelocity;
  /** Hidden */
  Vector3 _initialPosition;

  Property<Collider, int> collisionMask;

private:
  Vector3 _collisionPoint;
  Vector3 _planeIntersectionPoint;
  Vector3 _tempVector;
  Vector3 _tempVector2;
  Vector3 _tempVector3;
  Vector3 _tempVector4;
  Vector3 _edge;
  Vector3 _baseToVertex;
  Vector3 _destinationPoint;
  Vector3 _slidePlaneNormal;
  Vector3 _displacementVector;
  Vector3 _velocity;
  Vector3 _basePoint;
  float _epsilon;
  Vector3 _velocityWorld;
  Vector3 _normalizedVelocity;
  float _nearestDistance;
  int _collisionMask;

}; // end of class Collider

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLIDER_H
