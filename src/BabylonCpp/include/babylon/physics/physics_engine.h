#ifndef BABYLON_PHYSICS_PHYSICS_ENGINE_H
#define BABYLON_PHYSICS_PHYSICS_ENGINE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/physics/iphysics_engine.h>

namespace BABYLON {

class PhysicsRaycastResult;
FWD_CLASS_SPTR(PhysicsEngine)
FWD_CLASS_SPTR(PhysicsImpostor)
FWD_STRUCT_SPTR(PhysicsImpostorJoint)

/**
 * @brief Class used to control physics engine.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsEngine : public IPhysicsEngine {

public:
  /**
   * Global value used to control the smallest number supported by the simulation
   */
  static float Epsilon;

public:
  template <typename... Ts>
  static PhysicsEnginePtr New(Ts&&... args)
  {
    return std::shared_ptr<PhysicsEngine>(new PhysicsEngine(std::forward<Ts>(args)...));
  }
  ~PhysicsEngine() override; // = default

  /**
   * @brief Sets the gravity vector used by the simulation.
   * @param gravity defines the gravity vector to use
   */
  void setGravity(const Vector3& gravity) final;

  /**
   * @brief Set the time step of the physics engine.
   * Default is 1/60.
   * To slow it down, enter 1/600 for example.
   * To speed it up, 1/30
   * @param newTimeStep defines the new timestep to apply to this world.
   */
  void setTimeStep(float newTimeStep = 1.f / 60.f) final;

  /**
   * @brief Get the time step of the physics engine.
   * @returns the current time step
   */
  [[nodiscard]] float getTimeStep() const final;

  /**
   * @brief Set the sub time step of the physics engine.
   * Default is 0 meaning there is no sub steps
   * To increase physics resolution precision, set a small value (like 1 ms)
   * @param subTimeStep defines the new sub timestep used for physics resolution.
   */
  void setSubTimeStep(float subTimeStep = 0.f) final;

  /**
   * @brief Get the sub time step of the physics engine.
   * @returns the current sub time step
   */
  float getSubTimeStep() const final;

  /**
   * @brief Release all resources.
   */
  void dispose() final;

  /**
   * @brief Gets the name of the current physics plugin.
   * @returns the name of the plugin
   */
  [[nodiscard]] std::string getPhysicsPluginName() const final;

  /**
   * @brief Adding a new impostor for the impostor tracking.
   * This will be done by the impostor itself.
   * @param impostor the impostor to add
   */
  void addImpostor(PhysicsImpostor* impostor) final;

  /**
   * @brief Remove an impostor from the engine.
   * This impostor and its mesh will not longer be updated by the physics engine.
   * @param impostor the impostor to remove
   */
  void removeImpostor(PhysicsImpostor* impostor) final;

  /**
   * @brief Add a joint to the physics engine.
   * @param mainImpostor defines the main impostor to which the joint is added.
   * @param connectedImpostor defines the impostor that is connected to the main impostor using this
   * joint
   * @param joint defines the joint that will connect both impostors.
   */
  void addJoint(PhysicsImpostor* mainImpostor, PhysicsImpostor* connectedImpostor,
                const std::shared_ptr<PhysicsJoint>& joint) final;

  /**
   * @brief Removes a joint from the simulation.
   * @param mainImpostor defines the impostor used with the joint
   * @param connectedImpostor defines the other impostor connected to the main one by the joint
   * @param joint defines the joint to remove
   */
  void removeJoint(PhysicsImpostor* mainImpostor, PhysicsImpostor* connectedImpostor,
                   PhysicsJoint* joint) final;

  /**
   * @brief Called by the scene. No need to call it.
   * @param delta defines the timespan between frames
   */
  void _step(float delta) final;

  /**
   * @brief Gets the current plugin used to run the simulation.
   * @returns current plugin
   */
  IPhysicsEnginePlugin* getPhysicsPlugin() final;

  /**
   * @brief Gets the list of physic impostors.
   * @returns an array of PhysicsImpostor
   */
  std::vector<PhysicsImpostorPtr>& getImpostors() final;

  /**
   * @brief Gets the impostor for a physics enabled object.
   * @param object defines the object impersonated by the impostor
   * @returns the PhysicsImpostor or null if not found
   */
  PhysicsImpostor* getImpostorForPhysicsObject(IPhysicsEnabledObject* object) final;

  /**
   * @brief Gets the impostor for a physics body object.
   * @param body defines physics body used by the impostor
   * @returns the PhysicsImpostor or null if not found
   */
  PhysicsImpostor* getImpostorWithPhysicsBody(IPhysicsBody* body) final;

  /**
   * @brief Does a raycast in the physics world.
   * @param from when should the ray start?
   * @param to when should the ray end?
   * @returns PhysicsRaycastResult
   */
  PhysicsRaycastResult raycast(const Vector3& from, const Vector3& to) final;

  /**
   * @brief Returns whether or not the engine is initialized.
   */
  [[nodiscard]] bool isInitialized() const final;

protected:
  /**
   * @brief Creates a new Physics Engine.
   * @param gravity defines the gravity vector used by the simulation
   * @param _physicsPlugin defines the plugin to use (CannonJS by default)
   */
  PhysicsEngine(const std::optional<Vector3>& gravity = std::nullopt,
                IPhysicsEnginePlugin* physicsPlugin   = nullptr);

private:
  bool _initialized;
  IPhysicsEnginePlugin* _physicsPlugin;
  std::vector<PhysicsImpostorPtr> _impostors;
  std::vector<PhysicsImpostorJointPtr> _joints;
  float _subTimeStep;

}; // end of class PhysicsEngine

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_ENGINE_H
