#ifndef BABYLON_PHYSICS_IPHYSICS_ENGINE_PLUGIN_H
#define BABYLON_PHYSICS_IPHYSICS_ENGINE_PLUGIN_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT IPhysicsEnginePlugin {
  IWorld* world;
  std::string name;
  virtual void setGravity(const Vector3& gravity) = 0;
  virtual void setTimeStep(float timeStep)        = 0;
  virtual void executeStep(float delta, std::vector<PhysicsImpostor*> impostors)
    = 0; // not forgetting pre and post events
  virtual void applyImpulse(PhysicsImpostor* impostor, const Vector3& force,
                            const Vector3& contactPoint)
    = 0;
  virtual void applyForce(PhysicsImpostor* impostor, const Vector3& force,
                          const Vector3& contactPoint)
    = 0;
  virtual void generatePhysicsBody(PhysicsImpostor* impostor) = 0;
  virtual void removePhysicsBody(PhysicsImpostor* impostor)   = 0;
  virtual void generateJoint(PhysicsImpostorJoint* joint)     = 0;
  virtual void removeJoint(PhysicsImpostorJoint* joint)       = 0;
  virtual bool isSupported()                                  = 0;
  virtual void setTransformationFromPhysicsBody(PhysicsImpostor* impostor);
  virtual void setPhysicsBodyTransformation(PhysicsImpostor* impostor,
                                            const Vector3& newPosition,
                                            const Quaternion& newRotation)
    = 0;
  virtual void setLinearVelocity(PhysicsImpostor* impostor,
                                 const Vector3& velocity)
    = 0;
  virtual void setAngularVelocity(PhysicsImpostor* impostor,
                                  const Vector3& velocity)
    = 0;
  virtual Vector3 getLinearVelocity(PhysicsImpostor* impostor)  = 0;
  virtual Vector3 getAngularVelocity(PhysicsImpostor* impostor) = 0;
  virtual void setBodyMass(PhysicsImpostor* impostor, float mass) = 0;
  virtual void sleepBody(PhysicsImpostor* impostor)  = 0;
  virtual void wakeUpBody(PhysicsImpostor* impostor) = 0;
  // Joint Update
  virtual void updateDistanceJoint(DistanceJoint* joint, float maxDistance,
                                   float minDistance)
    = 0;
  virtual void setMotor(IMotorEnabledJoint* joint, float speed, float maxForce,
                        unsigned int motorIndex = 0)
    = 0;
  virtual void setLimit(IMotorEnabledJoint* joint, float upperLimit,
                        float lowerLimit, unsigned int motorIndex = 0)
    = 0;
  virtual void dispose() = 0;
}; // end of struct IPhysicsEnginePlugin

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_ENGINE_PLUGIN_H
