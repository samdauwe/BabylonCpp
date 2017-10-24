#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_SIMULATION_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_SIMULATION_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/entitycomponentsystem/world.h>
#include <babylon/extensions/navigation/crowd_collision_avoidance_system.h>
#include <babylon/extensions/navigation/crowd_mesh_updater_system.h>
#include <babylon/math/vector2.h>

namespace BABYLON {
namespace Extensions {

namespace RVO2 {
class RVOSimulator;
}

/**
 * @brief Represents a crowd simulation.
 */
class BABYLON_SHARED_EXPORT CrowdSimulation {

public:
  CrowdSimulation();
  ~CrowdSimulation();

  // Add the necessary systems to the world
  void initializeWorld();

  // Specify the global time step of the simulation.
  void setTimeStep(float timeStep);

  size_t addAgent(AbstractMesh* mesh);
  size_t addAgent(AbstractMesh* mesh, const BABYLON::Vector2& position);
  void setAgentGoal(size_t agentId, const BABYLON::Vector2& goal);
  void setAgentMaxSpeed(size_t agentId, float speed);

  /* Process the obstacles so that they are accounted for in the simulation. */
  void processObstacles();
  void computeRoadMap();

  // Return whether or not the simulation is running.
  bool isRunning() const;

  // Updates the simulation
  void update();

private:
  // Simulator instance
  std::unique_ptr<RVO2::RVOSimulator> _simulator;
  // The anax entity world
  ECS::World _world;
  // The collision avoidance system
  CrowdCollisionAvoidanceSystem _crowdCollisionAvoidanceSystem;
  // The mesh updater system
  CrowdMeshUpdaterSystem _crowdMeshUpdaterSystem;
  // The crowd agents
  std::vector<ECS::Entity> _agents;

}; // end of class CrowdSimulation

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_NAVIGATION_CROWD_SIMULATION_H
