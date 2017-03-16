#ifndef BABYLON_PHYSICS_IPHYSICS_BODY_H
#define BABYLON_PHYSICS_IPHYSICS_BODY_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IPhysicsBody {

  virtual void setPosition(const Vector3& newPosition)       = 0;
  virtual void setOrientation(const Quaternion& newRotation) = 0;
  virtual void setShapesDensity(float density)               = 0;
  virtual void setupMass(int mass)                           = 0;
  virtual float mass()                                       = 0;
  virtual void applyImpulse(const Vector3& position, const Vector3& force) = 0;
  virtual Vector3 angularVelocity()                        = 0;
  virtual void setAngularVelocity(const Vector3& velocity) = 0;
  virtual Vector3 linearVelocity()                         = 0;
  virtual void setLinearVelocity(const Vector3& velocity)  = 0;
  virtual void sleep()                                     = 0;
  virtual bool sleeping()                                  = 0;
  virtual void awake()                                     = 0;
  virtual void syncShapes()                                = 0;

}; // end of struct IPhysicsBody

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_BODY_H
