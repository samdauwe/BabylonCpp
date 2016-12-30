#ifndef OIMO_CONSTRAINT_CONTACT_MANIFOLD_POINT_H
#define OIMO_CONSTRAINT_CONTACT_MANIFOLD_POINT_H

#include <oimo/math/vec3.h>

namespace OIMO {

/**
 * @brief The class holds details of the contact point.
 */
struct ManifoldPoint {
  // Whether this manifold point is persisting or not.
  bool warmStarted = false;
  //  The position of this manifold point.
  Vec3 position;
  // The position in the first shape's coordinate.
  Vec3 localPoint1;
  //  The position in the second shape's coordinate.
  Vec3 localPoint2;
  // The normal vector of this manifold point.
  Vec3 normal;
  // The tangent vector of this manifold point.
  Vec3 tangent;
  // The binormal vector of this manifold point.
  Vec3 binormal;
  // The impulse in normal direction.
  float normalImpulse = 0.f;
  // The impulse in tangent direction.
  float tangentImpulse = 0.f;
  // The impulse in binormal direction.
  float binormalImpulse = 0.f;
  // The denominator in normal direction.
  float normalDenominator = 0.f;
  // The denominator in tangent direction.
  float tangentDenominator = 0.f;
  // The denominator in binormal direction.
  float binormalDenominator = 0.f;
  // The depth of penetration.
  float penetration = 0.f;
}; // end of struct ManifoldPoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_CONTACT_MANIFOLD_POINT_H