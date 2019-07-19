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
using PhysicsImpostorPtr = std::shared_ptr<PhysicsImpostor>;

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
  virtual void applyImpulse(const PhysicsImpostor& impostor,
                            const Vector3& force, const Vector3& contactPoint)
    = 0;
  virtual void applyForce(const PhysicsImpostor& impostor, const Vector3& force,
                          const Vector3& contactPoint)
    = 0;
  virtual void generatePhysicsBody(const PhysicsImpostor& impostor) = 0;
  virtual void removePhysicsBody(const PhysicsImpostor& impostor)   = 0;
  virtual void generateJoint(PhysicsImpostorJoint* joint)           = 0;
  virtual void removeJoint(PhysicsImpostorJoint* joint)             = 0;
  virtual bool isSupported()                                        = 0;
  virtual void setTransformationFromPhysicsBody(const PhysicsImpostor& impostor)
    = 0;
  virtual void setPhysicsBodyTransformation(const PhysicsImpostor& impostor,
                                            const Vector3& newPosition,
                                            const Quaternion& newRotation)
    = 0;
  virtual void setLinearVelocity(const PhysicsImpostor& impostor,
                                 const std::optional<Vector3>& velocity)
    = 0;
  virtual void setAngularVelocity(const PhysicsImpostor& impostor,
                                  const std::optional<Vector3>& velocity)
    = 0;
  virtual Vector3 getLinearVelocity(const PhysicsImpostor& impostor)    = 0;
  virtual Vector3 getAngularVelocity(const PhysicsImpostor& impostor)   = 0;
  virtual void setBodyMass(const PhysicsImpostor& impostor, float mass) = 0;
  virtual float getBodyMass(const PhysicsImpostor& impostor)            = 0;
  virtual float getBodyFriction(const PhysicsImpostor& impostor)        = 0;
  virtual void setBodyFriction(const PhysicsImpostor& impostor, float friction)
    = 0;
  virtual float getBodyRestitution(const PhysicsImpostor& impostor) = 0;
  virtual void setBodyRestitution(const PhysicsImpostor& impostor,
                                  float restitution)
    = 0;
  virtual float getBodyPressure(const PhysicsImpostor& impostor) = 0;
  virtual void setBodyPressure(const PhysicsImpostor& impostor, float pressure)
    = 0;
  virtual float getBodyStiffness(const PhysicsImpostor& impostor) = 0;
  virtual void setBodyStiffness(const PhysicsImpostor& impostor,
                                float stiffness)
    = 0;
  virtual size_t getBodyVelocityIterations(const PhysicsImpostor& impostor) = 0;
  virtual void setBodyVelocityIterations(const PhysicsImpostor& impostor,
                                         size_t velocityIterations)
    = 0;
  virtual size_t getBodyPositionIterations(const PhysicsImpostor& impostor) = 0;
  virtual void setBodyPositionIterations(const PhysicsImpostor& impostor,
                                         size_t positionIterations)
    = 0;
  virtual void appendAnchor(const PhysicsImpostor& impostor,
                            const PhysicsImpostorPtr& otherImpostor, int width,
                            int height, float influence,
                            bool noCollisionBetweenLinkedBodies)
    = 0;
  virtual void appendHook(const PhysicsImpostor& impostor,
                          const PhysicsImpostorPtr& otherImpostor, float length,
                          float influence, bool noCollisionBetweenLinkedBodies)
    = 0;
  virtual void sleepBody(const PhysicsImpostor& impostor)  = 0;
  virtual void wakeUpBody(const PhysicsImpostor& impostor) = 0;
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
  virtual float getRadius(const PhysicsImpostor& impostor) = 0;
  virtual void getBoxSizeToRef(const PhysicsImpostor& impostor, Vector3& result)
    = 0;
  virtual void syncMeshWithImpostor(AbstractMesh* mesh,
                                    const PhysicsImpostor& impostor)
    = 0;
  virtual void dispose() = 0;
}; // end of struct IPhysicsEnginePlugin

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_ENGINE_PLUGIN_H
