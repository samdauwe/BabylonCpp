#ifndef BABYLON_EXTENSIONS_NAVIGATION_CROWD_SIMULATION_H
#define BABYLON_EXTENSIONS_NAVIGATION_CROWD_SIMULATION_H

#include <babylon/babylon_api.h>
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

  /* Add the necessary systems to the world */
  void initializeWorld();

  /* Specify the global time step of the simulation. */
  void setTimeStep(float timeStep);

  /* Agent management functions */
  size_t addAgent(const AbstractMeshPtr& mesh);
  size_t addAgent(const AbstractMeshPtr& mesh, const Vector2& position);
  size_t addAgent(const AbstractMeshPtr& mesh, const Vector3& position);
  void setAgentGoal(size_t agentId, const BABYLON::Vector2& goal);
  void setAgentMaxSpeed(size_t agentId, float speed);

  /* Obstacle management functions */
  void addObstacleByBoundingBox(const AbstractMeshPtr& mesh,
                                const Vector3& position, bool isVisible = true);

  /* Process the obstacles so that they are accounted for in the simulation. */
  void processObstacles();

  /* Add a roadmap vertex. */
  void addWayPoint(const BABYLON::Vector2& waypoint);
  void computeRoadMap();

  /* Set the simulation precision. */
  void setPrecision(unsigned int precision);

  /* Return whether or not the simulation is running. */
  bool isRunning() const;

  /* Updates the simulation */
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
