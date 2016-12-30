#ifndef BABYLON_COLLISIONS_COLLIDER_H
#define BABYLON_COLLISIONS_COLLIDER_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Collider {

public:
  Collider();
  ~Collider();

  /** Statics **/
  static bool IntersectBoxAASphere(const Vector3& boxMin, const Vector3& boxMax,
                                   const Vector3& sphereCenter,
                                   float sphereRadius);
  static LowestRoot GetLowestRoot(float a, float b, float c, float maxR);

  /** Methods **/
  void _initialize(Vector3& source, Vector3& dir, float e);
  bool _checkPointInTriangle(const Vector3& point, const Vector3& pa,
                             const Vector3& pb, const Vector3& pc,
                             const Vector3& n);
  bool _canDoCollision(const Vector3& sphereCenter, float sphereRadius,
                       const Vector3& vecMin, const Vector3& vecMax) const;
  void _testTriangle(size_t faceIndex, std::vector<Plane>& trianglePlaneArray,
                     Vector3& p1, Vector3& p2, Vector3& p3, bool hasMaterial);
  void _collide(std::vector<Plane>& trianglePlaneArray,
                const std::vector<Vector3> pts, const Uint32Array& indices,
                size_t indexStart, size_t indexEnd, unsigned int decal,
                bool hasMaterial);
  void _getResponse(Vector3& pos, Vector3& vel);

public:
  Vector3 radius;
  unsigned int retry;
  Vector3 velocity;
  Vector3 basePoint;
  float epsilon;
  bool collisionFound;
  float velocityWorldLength;
  Vector3 basePointWorld;
  Vector3 velocityWorld;
  Vector3 normalizedVelocity;
  Vector3 initialVelocity;
  Vector3 initialPosition;
  float nearestDistance;
  Vector3 intersectionPoint;
  bool intersectionPointSet;
  AbstractMesh* collidedMesh;
  unsigned int collidedMeshId;

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

}; // end of class Collider

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_COLLIDER_H
