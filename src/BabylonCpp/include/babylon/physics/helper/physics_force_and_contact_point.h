#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_FORCE_AND_CONTACT_POINT_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_FORCE_AND_CONTACT_POINT_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhysicsForceAndContactPoint {
  Vector3 force;
  Vector3 contactPoint;
}; // end of struct PhysicsForceAndContactPoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_FORCE_AND_CONTACT_POINT_H
