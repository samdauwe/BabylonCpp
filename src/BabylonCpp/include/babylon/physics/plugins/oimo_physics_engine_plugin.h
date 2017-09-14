#ifndef BABYLON_PHYSICS_PLUGINS_OIMO_PHYSICS_ENGINE_PLUGIN_H
#define BABYLON_PHYSICS_PLUGINS_OIMO_PHYSICS_ENGINE_PLUGIN_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>

namespace OIMO {
class RigidBody;
class Shape;
class World;
} // namespace OIMO

namespace BABYLON {

class BABYLON_SHARED_EXPORT OimoPhysicsEnginePlugin {

public:
  OimoPhysicsEnginePlugin(unsigned int iterations);
  ~OimoPhysicsEnginePlugin();

  void setGravity(const Vector3& gravity);
  void setTimeStep(float timeStep);
  void executeStep(float delta, const vector_t<PhysicsImpostor*>& impostors);
  void applyImpulse(PhysicsImpostor* impostor, const Vector3& force,
                    const Vector3& contactPoint);
  void applyForce(PhysicsImpostor* impostor, const Vector3& force,
                  const Vector3& contactPoint);
  void generatePhysicsBody(PhysicsImpostor* impostor);
  void removePhysicsBody(PhysicsImpostor* impostor);
  void generateJoint(PhysicsImpostorJoint* impostorJoint);
  void removeJoint(PhysicsImpostorJoint* impostorJoint);
  bool isSupported() const;
  void setTransformationFromPhysicsBody(PhysicsImpostor* impostor);
  void setPhysicsBodyTransformation(PhysicsImpostor* impostor,
                                    const Vector3& newPosition,
                                    const Quaternion& newRotation);
  void setLinearVelocity(PhysicsImpostor* impostor, const Vector3& velocity);
  void setAngularVelocity(PhysicsImpostor* impostor, const Vector3& velocity);
  Vector3 getLinearVelocity(PhysicsImpostor* impostor);
  Vector3 getAngularVelocity(PhysicsImpostor* impostor);
  void setBodyMass(PhysicsImpostor* impostor, float mass);
  void sleepBody(PhysicsImpostor* impostor);
  void wakeUpBody(PhysicsImpostor* impostor);
  void updateDistanceJoint(IMotorEnabledJoint* joint, float maxDistance,
                           float minDistance);
  void setMotor(IMotorEnabledJoint* joint, float speed, float maxForce,
                unsigned int motorIndex);
  void setLimit(IMotorEnabledJoint* joint, float upperLimit, float lowerLimit,
                unsigned int motorIndex);
  void dispose();

private:
  OIMO::Shape* getLastShape(OIMO::RigidBody* body);

private:
  std::unique_ptr<IWorld> world;
  string_t name;
  std::unordered_map<string_t, PhysicsImpostor*> _tmpImpostorsArray;
  Vector3 _tmpPositionVector;

}; // end of class OimoPhysicsEnginePlugin

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PLUGINS_OIMO_PHYSICS_ENGINE_PLUGIN_H
