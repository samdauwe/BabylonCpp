#ifndef BABYLON_PHYSICS_IPHYSICS_ENGINE_H
#define BABYLON_PHYSICS_IPHYSICS_ENGINE_H

#include <babylon/babylon_api.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

struct IPhysicsBody;
struct IPhysicsEnabledObject;
struct IPhysicsEnginePlugin;
class PhysicsImpostor;
class PhysicsJoint;
class PhysicsRaycastResult;
using PhysicsImpostorPtr = std::shared_ptr<PhysicsImpostor>;

/**
 * @brief Interface used to define a physics engine.
 * @see http://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT IPhysicsEngine {
  virtual ~IPhysicsEngine() = default;
  /**
   * Gets the gravity vector used by the simulation
   */
  Vector3 gravity;

  /**
   * @brief Sets the gravity vector used by the simulation
   * @param gravity defines the gravity vector to use
   */
  virtual void setGravity(const Vector3& gravity) = 0;

  /**
   * @brief Set the time step of the physics engine.
   * Default is 1/60.
   * To slow it down, enter 1/600 for example.
   * To speed it up, 1/30
   * @param newTimeStep the new timestep to apply to this world.
   */
  virtual void setTimeStep(float newTimeStep = 1.f / 60.f) = 0;

  /**
   * @brief Get the time step of the physics engine.
   * @returns the current time step
   */
  [[nodiscard]] virtual float getTimeStep() const = 0;

  /**
   * @brief Set the sub time step of the physics engine.
   * Default is 0 meaning there is no sub steps
   * To increase physics resolution precision, set a small value (like 1 ms)
   * @param subTimeStep defines the new sub timestep used for physics resolution.
   */
  virtual void setSubTimeStep(float subTimeStep = 0.f) = 0;

  /**
   * @brief Get the sub time step of the physics engine.
   * @returns the current sub time step
   */
  virtual float getSubTimeStep() const = 0;

  /**
   * @brief Release all resources
   */
  virtual void dispose() = 0;

  /**
   * @brief Gets the name of the current physics plugin
   * @returns the name of the plugin
   */
  [[nodiscard]] virtual std::string getPhysicsPluginName() const = 0;

  /**
   * @brief Adding a new impostor for the impostor tracking.
   * This will be done by the impostor itself.
   * @param impostor the impostor to add
   */
  virtual void addImpostor(PhysicsImpostor* impostor) = 0;

  /**
   * @brief Remove an impostor from the engine.
   * This impostor and its mesh will not longer be updated by the physics
   * engine.
   * @param impostor the impostor to remove
   */
  virtual void removeImpostor(PhysicsImpostor* impostor) = 0;

  /**
   * @brief Add a joint to the physics engine
   * @param mainImpostor defines the main impostor to which the joint is added.
   * @param connectedImpostor defines the impostor that is connected to the main
   * impostor using this joint
   * @param joint defines the joint that will connect both impostors.
   */
  virtual void addJoint(PhysicsImpostor* mainImpostor, PhysicsImpostor* connectedImpostor,
                        const std::shared_ptr<PhysicsJoint>& joint)
    = 0;

  /**
   * @brief Removes a joint from the simulation
   * @param mainImpostor defines the impostor used with the joint
   * @param connectedImpostor defines the other impostor connected to the main
   * one by the joint
   * @param joint defines the joint to remove
   */
  virtual void removeJoint(PhysicsImpostor* mainImpostor, PhysicsImpostor* connectedImpostor,
                           PhysicsJoint* joint)
    = 0;

  /**
   * @brief Gets the current plugin used to run the simulation
   * @returns current plugin
   */
  virtual IPhysicsEnginePlugin* getPhysicsPlugin() = 0;

  /**
   * @brief Gets the list of physic impostors
   * @returns an array of PhysicsImpostor
   */
  virtual std::vector<PhysicsImpostorPtr>& getImpostors() = 0;

  /**
   * @brief Gets the impostor for a physics enabled object
   * @param object defines the object impersonated by the impostor
   * @returns the PhysicsImpostor or null if not found
   */
  virtual PhysicsImpostor* getImpostorForPhysicsObject(IPhysicsEnabledObject* object) = 0;

  /**
   * @brief Gets the impostor for a physics body object
   * @param body defines physics body used by the impostor
   * @returns the PhysicsImpostor or null if not found
   */
  virtual PhysicsImpostor* getImpostorWithPhysicsBody(IPhysicsBody* body) = 0;

  /**
   * @brief Does a raycast in the physics world.
   * @param from when should the ray start?
   * @param to when should the ray end?
   * @returns PhysicsRaycastResult
   */
  virtual PhysicsRaycastResult raycast(const Vector3& from, const Vector3& to) = 0;

  /**
   * @brief Called by the scene. No need to call it.
   * @param delta defines the timespam between frames
   */
  virtual void _step(float delta) = 0;

  /**
   * @brief Returns whether or not the engine is initialized.
   */
  [[nodiscard]] virtual bool isInitialized() const = 0;

}; // end of struct IPhysicsEngine

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_IPHYSICS_ENGINE_H
