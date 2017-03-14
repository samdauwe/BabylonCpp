#ifndef BABYLON_PHYSICS_IPHYSICS_BODY_H
#define BABYLON_PHYSICS_IPHYSICS_BODY_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IPhysicsBody {

  virtual float getMass() = 0;
  virtual void applyImpulse(const Vector3& position, const Vector3& force) = 0;

}; // end of struct IPhysicsBody

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_BODY_H
