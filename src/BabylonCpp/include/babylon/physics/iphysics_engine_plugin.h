#ifndef BABYLON_PHYSICS_IPHYSICS_ENGINE_PLUGIN_H
#define BABYLON_PHYSICS_IPHYSICS_ENGINE_PLUGIN_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class AbstractMesh;
class DistanceJoint;
struct IMotorEnabledJoint;
class Mesh;
class PhysicsImpostor;
struct PhysicsImpostorJoint;
class PhysicsRaycastResult;
class Quaternion;
class Vector3;

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT IPhysicsEnginePlugin {
  Mesh* world;
  std::string name;
  virtual void setGravity(const Vector3& gravity) = 0;
  virtual void setTimeStep(float timeStep)        = 0;
  virtual float getTimeStep() const               = 0;
  virtual void
  executeStep(float delta,
              const std::vector<std::unique_ptr<PhysicsImpostor>>& impostors)
    = 0; // not forgetting pre and post events
  virtual void applyImpulse(PhysicsImpostor* impostor, const Vector3& force,
                            const Vector3& contactPoint)
    = 0;
  virtual void applyForce(PhysicsImpostor* impostor, const Vector3& force,
                          const Vector3& contactPoint)
    = 0;
  virtual void generatePhysicsBody(PhysicsImpostor* impostor)              = 0;
  virtual void removePhysicsBody(PhysicsImpostor* impostor)                = 0;
  virtual void generateJoint(PhysicsImpostorJoint* joint)                  = 0;
  virtual void removeJoint(PhysicsImpostorJoint* joint)                    = 0;
  virtual bool isSupported()                                               = 0;
  virtual void setTransformationFromPhysicsBody(PhysicsImpostor* impostor) = 0;
  virtual void setPhysicsBodyTransformation(PhysicsImpostor* impostor,
                                            const Vector3& newPosition,
                                            const Quaternion& newRotation)
    = 0;
  virtual void setLinearVelocity(PhysicsImpostor* impostor,
                                 const std::optional<Vector3>& velocity)
    = 0;
  virtual void setAngularVelocity(PhysicsImpostor* impostor,
                                  const std::optional<Vector3>& velocity)
    = 0;
  virtual Vector3 getLinearVelocity(PhysicsImpostor* impostor)            = 0;
  virtual Vector3 getAngularVelocity(PhysicsImpostor* impostor)           = 0;
  virtual void setBodyMass(PhysicsImpostor* impostor, float mass)         = 0;
  virtual float getBodyMass(const PhysicsImpostor* impostor)              = 0;
  virtual float getBodyFriction(const PhysicsImpostor* impostor)          = 0;
  virtual void setBodyFriction(PhysicsImpostor* impostor, float friction) = 0;
  virtual float getBodyRestitution(const PhysicsImpostor* impostor)       = 0;
  virtual void setBodyRestitution(PhysicsImpostor* impostor, float restitution)
    = 0;
  virtual void sleepBody(PhysicsImpostor* impostor)  = 0;
  virtual void wakeUpBody(PhysicsImpostor* impostor) = 0;
  virtual PhysicsRaycastResult raycast(const Vector3& from, const Vector3& to)
    = 0;
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
  virtual float getRadius(const PhysicsImpostor* impostor)                 = 0;
  virtual void getBoxSizeToRef(PhysicsImpostor* impostor, Vector3& result) = 0;
  virtual void syncMeshWithImpostor(AbstractMesh* mesh,
                                    PhysicsImpostor* impostor)
    = 0;
  virtual void dispose() = 0;
}; // end of struct IPhysicsEnginePlugin

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_ENGINE_PLUGIN_H
