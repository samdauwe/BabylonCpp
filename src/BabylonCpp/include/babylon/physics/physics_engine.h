#ifndef BABYLON_PHYSICS_PHYSICS_ENGINE_H
#define BABYLON_PHYSICS_PHYSICS_ENGINE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PhysicsEngine : public IDisposable {

public:
  PhysicsEngine(const Vector3& gravity = Vector3(0.f, -9.807f, 0.f),
                IPhysicsEnginePlugin* physicsPlugin = nullptr);
  virtual ~PhysicsEngine();

  void setGravity(const Vector3& gravity);

  /**
   * Set the time step of the physics engine.
   * default is 1/60.
   * To slow it down, enter 1/600 for example.
   * To speed it up, 1/30
   * @param {number} newTimeStep the new timestep to apply to this world.
   */
  void setTimeStep(float newTimeStep = 1.f / 60.f);

  void dispose(bool doNotRecurse = false) override;
  std::string getPhysicsPluginName() const;

  /**
   * Adding a new impostor for the impostor tracking.
   * This will be done by the impostor itself.
   * @param {PhysicsImpostor} impostor the impostor to add
   */
  void addImpostor(PhysicsImpostor* impostor);

  /**
   * Remove an impostor from the engine.
   * This impostor and its mesh will not longer be updated by the physics
   * engine.
   * @param {PhysicsImpostor} impostor the impostor to remove
   */
  void removeImpostor(PhysicsImpostor* impostor);

  /**
   * Add a joint to the physics engine
   * @param {PhysicsImpostor} mainImpostor the main impostor to which the joint
   * is added.
   * @param {PhysicsImpostor} connectedImpostor the impostor that is connected
   * to the main impostor using this joint
   * @param {PhysicsJoint} the joint that will connect both impostors.
   */
  void addJoint(PhysicsImpostor* mainImpostor,
                PhysicsImpostor* connectedImpostor,
                const std::shared_ptr<PhysicsJoint>& joint);

  void removeJoint(PhysicsImpostor* mainImpostor,
                   PhysicsImpostor* connectedImpostor, PhysicsJoint* joint);

  /**
   * Called by the scene. no need to call it.
   */
  void _step(float delta);

  IPhysicsEnginePlugin* getPhysicsPlugin();

  PhysicsImpostor* getImpostorForPhysicsObject(IPhysicsEnabledObject* object);

  PhysicsImpostor* getImpostorWithPhysicsBody(AbstractMesh* body);

  bool isInitialized() const;

public:
  Vector3 gravity;

private:
  bool _initialized;
  IPhysicsEnginePlugin* _physicsPlugin;
  std::vector<PhysicsImpostor*> _impostors;
  std::vector<PhysicsImpostorJoint*> _joints;

}; // end of class PhysicsEngine

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_ENGINE_H
