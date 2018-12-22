#ifndef BABYLON_PHYSICS_PHYSICS_ENGINE_H
#define BABYLON_PHYSICS_PHYSICS_ENGINE_H

#include <babylon/babylon_api.h>
#include <babylon/physics/iphysics_engine.h>

namespace BABYLON {

class PhysicsEngine;
struct PhysicsImpostorJoint;
using PhysicsEnginePtr = std::shared_ptr<PhysicsEngine>;

/**
 * @brief Class used to control physics engine.
 * @see http://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsEngine : public IPhysicsEngine {

public:
  /**
   * Global value used to control the smallest number supported by the
   * simulation
   */
  static float Epsilon;

public:
  template <typename... Ts>
  static PhysicsEnginePtr New(Ts&&... args)
  {
    return std::shared_ptr<PhysicsEngine>(
      new PhysicsEngine(std::forward<Ts>(args)...));
  }
  virtual ~PhysicsEngine();

  /**
   * @brief Sets the gravity vector used by the simulation.
   * @param gravity defines the gravity vector to use
   */
  void setGravity(const Vector3& gravity) override;

  /**
   * @brief Set the time step of the physics engine.
   * Default is 1/60.
   * To slow it down, enter 1/600 for example.
   * To speed it up, 1/30
   * @param newTimeStep defines the new timestep to apply to this world.
   */
  void setTimeStep(float newTimeStep = 1.f / 60.f) override;

  /**
   * @brief Get the time step of the physics engine.
   * @returns the current time step
   */
  float getTimeStep() const override;

  /**
   * @brief Release all resources.
   */
  void dispose() override;

  /**
   * @brief Gets the name of the current physics plugin.
   * @returns the name of the plugin
   */
  std::string getPhysicsPluginName() const override;

  /**
   * @brief Adding a new impostor for the impostor tracking.
   * This will be done by the impostor itself.
   * @param impostor the impostor to add
   */
  void addImpostor(PhysicsImpostor* impostor) override;

  /**
   * @brief Remove an impostor from the engine.
   * This impostor and its mesh will not longer be updated by the physics
   * engine.
   * @param impostor the impostor to remove
   */
  void removeImpostor(PhysicsImpostor* impostor) override;

  /**
   * @brief Add a joint to the physics engine.
   * @param mainImpostor defines the main impostor to which the joint is added.
   * @param connectedImpostor defines the impostor that is connected to the main
   * impostor using this joint
   * @param joint defines the joint that will connect both impostors.
   */
  void addJoint(PhysicsImpostor* mainImpostor,
                PhysicsImpostor* connectedImpostor,
                const std::shared_ptr<PhysicsJoint>& joint) override;

  /**
   * @brief Removes a joint from the simulation.
   * @param mainImpostor defines the impostor used with the joint
   * @param connectedImpostor defines the other impostor connected to the main
   * one by the joint
   * @param joint defines the joint to remove
   */
  void removeJoint(PhysicsImpostor* mainImpostor,
                   PhysicsImpostor* connectedImpostor,
                   PhysicsJoint* joint) override;

  /**
   * @brief Called by the scene. No need to call it.
   * @param delta defines the timespam between frames
   */
  void _step(float delta) override;

  /**
   * @brief Gets the current plugin used to run the simulation.
   * @returns current plugin
   */
  IPhysicsEnginePlugin* getPhysicsPlugin() override;

  /**
   * @brief Gets the list of physic impostors.
   * @returns an array of PhysicsImpostor
   */
  std::vector<std::shared_ptr<PhysicsImpostor>>& getImpostors() override;

  /**
   * @brief Gets the impostor for a physics enabled object.
   * @param object defines the object impersonated by the impostor
   * @returns the PhysicsImpostor or null if not found
   */
  PhysicsImpostor*
  getImpostorForPhysicsObject(IPhysicsEnabledObject* object) override;

  /**
   * @brief Gets the impostor for a physics body object.
   * @param body defines physics body used by the impostor
   * @returns the PhysicsImpostor or null if not found
   */
  PhysicsImpostor* getImpostorWithPhysicsBody(IPhysicsBody* body) override;

  /**
   * @brief Returns whether or not the engine is initialized.
   */
  bool isInitialized() const override;

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
  std::vector<std::shared_ptr<PhysicsImpostor>> _impostors;
  std::vector<std::shared_ptr<PhysicsImpostorJoint>> _joints;

}; // end of class PhysicsEngine

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_PHYSICS_ENGINE_H
